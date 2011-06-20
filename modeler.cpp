#include "modeler.h"
#include "mystring.h"
#include "constants.h"
#include "mainwindow.h"
#include <QtGui>

Modeler::Modeler(
        Browser *browserP,
        LogWidget *logWidgetP,
        QSet<QString> *blacklistP,
        QSet<QString> *whitelistP,
        bool *stopFlagP) :
        browser(browserP),
        logWidget(logWidgetP),
        blacklist(blacklistP),
        whitelist(whitelistP),
        stopFlag(stopFlagP)
{
}

bool Modeler::run(Data *data, const QString &url)
{
    QString label = "init";

    //determine original title
    QString code = "document.title";
    data->originalTitle = MyString::truncate(browser->JStoString(code));

    recurse(
        data,
        url,
        label, //affordance label
        label, //action label
        0, //state
        ARROW_TYPE_INIT, //arrow type
        0 //arrow flags
    );

    if (*stopFlag)
    {
        logWidget->push("===stopped===");
        *stopFlag = false;
    }

    return true;
}

void Modeler::recurse(
        Data *data,
        const QString &url,
        QString &affordanceLabel,
        QString &actionLabel,
        int parentStateId,
        int arrowType,
        int arrowFlags)
{
    //exit condition 1: limit total number of states
    if (data->counter >= STATE_MAX)
    {
        logWidget->push("===state max limit===\n");
        *stopFlag = true;
        return;
    }

    //exit condition 2: bail out if stopFlag set
    if (*stopFlag)
    {
        qDebug() << "stop flag";
        return;
    }

    QString code;
    code = "state()";
    QString state = browser->JStoString(code);

    code = "document.title";
    QString stateTitle = MyString::truncate(browser->JStoString(code));

    if (stateTitle == data->originalTitle)
    {
        code = "firstHeadings()";
        QString ret = browser->JStoString(code);
        qDebug() << "headings: " << ret;
        QStringList headings = ret.split("|");
        if (headings.count() >= 2)
        {
            QString h1, h2;
            h1 = headings.at(0);
            h2 = headings.at(1);
            if (h1 != data->originalTitle)
            {
                stateTitle = h1;
                qDebug() << "state title now: " << h1;
            }
            else if (h2 != data->originalTitle)
            {
                stateTitle = h2;
                qDebug() << "state title now: " << h2;
            }
        }
    }

    //keeping track of lastLocalUrl
    if (linkType(data->originalUrl, url) != LINK_EXTERNAL &&
        locationInScope(
                browser->url().toString(),
                data->originalUrl))
    {
        data->lastLocalUrl = url;
    }

    int stateId = 0;

    //arrows to known states
    if (data->states.contains(state))
    {
        (data->affordanceCounter)++;
        Arrow affordanceArrow(data->affordanceCounter, parentStateId, data->states[state], arrowType, true, affordanceLabel);

        affordanceArrow.setFlags(arrowFlags);

        //tbr: derive from affordance graph instead
        Arrow actionArrow((data->actionCounter) + 1, parentStateId, data->states[state], ARROW_TYPE_ACTION, true, actionLabel);

        if (affordanceLabel != "init")
        {
            data->affordanceEdges.push_back(affordanceArrow);
            data->mapAffordanceToAbstractEdges.push_back(qMakePair(affordanceArrow, actionArrow));
        }

        if (!data->actionEdges.contains(actionArrow))
        {
            (data->actionCounter)++; //delayed increment
            if(actionLabel != "init")
            {
                data->actionEdges.push_back(actionArrow);
                data->abstractEdges.push_back(actionArrow); //interim: abstract == action graph
                data->mapActionToAbstractEdges.push_back(qMakePair(actionArrow, actionArrow));//mapping
            }
        }

        if (
            linkType(data->originalUrl, url) == LINK_EXTERNAL ||
            !locationInScope(
                    browser->url().toString(),
                    data->originalUrl))
        {
            QString log = "===transported to out-of-scope address ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            logWidget->push(log);
            browser->navigate(data->lastLocalUrl);
        }
        return;
    }
    else //all other arrows
    {
        //don't create nodes if out of scope and skipOutOfScopeUrls == true
        if (
                data->skipOutOfScopeUrls == true && (
                    this->linkType(data->originalUrl, url) == LINK_EXTERNAL ||
                    !locationInScope(
                        browser->url().toString(),
                        data->originalUrl))
                )
        {
            QString log = "===omitted out-of-scope address ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            logWidget->push(log);
            browser->navigate(data->lastLocalUrl);
            return;
        }

        (data->counter)++;
        (data->affordanceCounter)++;
        stateId = data->counter;
        data->states.insert(state, stateId);

        QString log = "===added state ";
        log += QString::number(data->counter - 1);
        log += " ";
        log += stateTitle;
        log += "===";
        logWidget->push(log);

        data->affordanceStateTitles.insert(stateId, stateTitle);
        data->actionStateTitles.insert(stateId, stateTitle);
        data->abstractStateTitles.insert(stateId, stateTitle);


        data->affordanceStates.push_back(State(stateId, MyString::makeState(stateId, stateTitle), state));
        data->actionStates.push_back(State(stateId, MyString::makeState(stateId, stateTitle), state));
        data->abstractStates.push_back(State(stateId, MyString::makeState(stateId, stateTitle), state));

        Arrow affordanceArrow(data->affordanceCounter, parentStateId, stateId, arrowType, true, affordanceLabel);

        affordanceArrow.setFlags(arrowFlags);

        Arrow actionArrow((data->actionCounter) + 1, parentStateId, stateId, ARROW_TYPE_ACTION, true, actionLabel);

        //mapping
        if(affordanceLabel != "init")
        {
            data->affordanceEdges.push_back(affordanceArrow);
            data->mapAffordanceToAbstractEdges.append(qMakePair(affordanceArrow, actionArrow));
        }
        QString affordanceStateTitle, actionStateTitle;
        affordanceStateTitle = (data->affordanceStateTitles.count(stateId)) ?
                               MyString::makeState(stateId, data->affordanceStateTitles[stateId]) :
                            "Affordance state " + QString::number(stateId);
        actionStateTitle = (data->actionStateTitles.count(stateId)) ?
                           MyString::makeState(stateId, data->actionStateTitles[stateId]) :
                        "Action state " + QString::number(stateId);

        data->mapAffordanceToAbstractNodes.push_back(qMakePair(State(stateId, affordanceStateTitle, state), State(stateId, actionStateTitle, state)));
        data->mapActionToAbstractNodes.push_back(qMakePair(State(stateId, actionStateTitle, state), State(stateId, actionStateTitle, state)));

        //end mapping

        if (!data->actionEdges.contains(actionArrow))
        {
            (data->actionCounter)++; //delayed increment
            data->actionStateTitles.insert(data->actionCounter, stateTitle);
            if(actionLabel != "init")
            {
                data->actionEdges.push_back(actionArrow);
                data->abstractEdges.push_back(actionArrow);
                data->mapActionToAbstractEdges.push_back(qMakePair(actionArrow, actionArrow));
            }
        }

        if (this->linkType(data->originalUrl, url) == LINK_EXTERNAL ||
            !locationInScope(
                    browser->url().toString(),
                    data->originalUrl))
        {
            QString log = "===transported to out-of-scope address ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            this->logWidget->push(log);
            browser->navigate(data->lastLocalUrl);
            return;
        }
    }

    browser->wait(true);

    //actions
    code = "nodeCount()";
    int count = browser->JStoInt(code);

    QString origin;
    origin = data->lastLocalUrl;

    for (int i = 0; i < count; i++)
    {
        if (!handleAction(data, i, stateId, arrowType, origin))
            break;
    }

    //hyperlinks
    code = "hyperlinkCount()";
    count = browser->JStoInt(code);

    for (int i = 0; i < count; i++)
    {
        if (!handleHyperlink(data, i, url, stateId, arrowType, origin))
            break;
    }

}

int Modeler::linkType(const QString &original, const QString &href)
{
    //internal link
    if (href.startsWith('#'))
    {
        return LINK_INTERNAL;
    }

    //relative link
    QUrl hrefUrl(href);
    if (hrefUrl.isRelative())
    {
        return LINK_RELATIVE;
    }

    QUrl currentUrl(original);
    QString currentHost, hrefHost, hrefPath;
    currentHost = currentUrl.host();
    hrefHost = hrefUrl.host();
    hrefPath = hrefUrl.path();

    //same-host link
    if (currentHost == hrefHost || hrefHost.isEmpty())
    {
        return LINK_SAME_HOST;
    }

    //external link
    return LINK_EXTERNAL;
}

QString Modeler::linkLabel(const QString &current, const QString &href)
{
    int type = linkType(current, href);

    QUrl hrefUrl(href);
    QString hrefPath = hrefUrl.path();
    QString hrefHost = hrefUrl.host();

    switch (type)
    {
    case LINK_INTERNAL:
        return href;
        break;
    case LINK_RELATIVE:
        return href;
        break;
    case LINK_SAME_HOST:
        return hrefPath;
        break;
    case LINK_EXTERNAL:
        return hrefHost;
        break;
    default:
        return "link";
    }
}

bool Modeler::listCheck(const QString &url)
{
    //blacklist
    QString field;
    QSetIterator<QString> itBlacklist(*blacklist);
    while (itBlacklist.hasNext())
    {
        field = itBlacklist.next();
        QRegExp rx(field);
        if (!rx.isValid())
        {
            qDebug() << "blacklist check: " << url << " is not a valid regex";
            continue;
        }
        if (rx.indexIn(url) != -1) // exit if matched
        {
            qDebug() << "blacklist check: " << url << " blacklisted";
            return false;
        }
    }

    //whitelist
    QSetIterator<QString> itWhitelist(*whitelist);
    if (!whitelist->isEmpty())
    {
        bool pass = false;
        while (itWhitelist.hasNext())
        {
            field = itWhitelist.next();
            QRegExp rx(field);
            if (!rx.isValid())
            {
                qDebug() << "whitelist check: " << url << " is not a valid regex";
                continue;
            }

            if (rx.indexIn(url) != -1) // set pass to true if matched
            {
                pass = true;
                break;
            }
        }
        if (!pass)
        {
            qDebug() << url << " not whitelisted";
            return false;
        }
    }

    return true;
}

bool Modeler::locationInScope(const QString &currentLocation, const QString &original)
{
    if (!listCheck(currentLocation))
    {
        return false;
    }

    int lastSlash = original.lastIndexOf("/");
    QString originalPathSegment = original.left(lastSlash);

    return currentLocation.startsWith(originalPathSegment);
}

bool Modeler::handleAction(Data *data, int index, int stateId, int arrowType, const QString &origin)
{
    if (*stopFlag)
    {
        return false;
    }

    QString code, affordanceLabel, actionLabel;

    //determine nodename
    code = "nodeName(";
    code += QString::number(index);
    code += ")";
    QString nodeName = browser->JStoString(code);

    //determine node text
    code = "nodeText(";
    code += QString::number(index);
    code += ")";
    QString nodeText = browser->JStoString(code);

    if (
        (nodeName.isEmpty() &&
        nodeText.isEmpty()) ||
        (nodeName == "\"\"" &&
        nodeText == "\"\""))
    {
        qDebug() << "Bailing out - node with empty name and text";
        browser->wait(true, 2);

        //tbd: bail out?
        return false; //exp
    }

    qDebug() << "Processing #" << index << " - node " << nodeName << " - label " << nodeText << "\n";

    for (int j = TRIGGER_CLICK; j < TRIGGER_INIT; j++)
    {
        /*
        //scroll to node
        code = "scrollToNode(";
        code += QString::number(index);
        code += ")";
        int verticalOffset = browser->JStoInt(code);
        qDebug() << "Vertical offset for scrolling: " << verticalOffset;
        */

        //position mouse above node
        code = "nodePosition(";
        code += QString::number(index);
        code += ")";
        QString posString = browser->JStoString(code);

        qDebug() << "local pos string: " << posString;

        int x, y;
        x = y = -1;
        bool positionKnown = false;
        QRegExp match("(^[^:]+):([^:]+)$");
        if ((match.isValid()) && (match.indexIn(posString) != -1))
        {
                QString xString, yString;
                xString = match.cap(1);
                yString = match.cap(2);
                x = xString.toInt();
                y = yString.toInt();

                positionKnown = (x == 0 && x == 0) ? false : true;
        }

        QPoint globalPos;
        if (positionKnown)
        {
            QPoint localPos;
            localPos.setX(x + 2); //just inside
            localPos.setY(y + 2); //just inside
            globalPos = browser->mapToGlobal(localPos);

            qDebug() << "global pos: x" << globalPos.x() << ":y" << globalPos.y();

            QCursor cursor(Qt::CrossCursor);
            cursor.setPos(globalPos);
            QApplication::setOverrideCursor(cursor);
        }

        QString result;

        //special case: click event

        if (positionKnown && j == TRIGGER_CLICK)
        {
            QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, globalPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QCoreApplication::postEvent(browser, event);
            result = "===posted click event by raising button press at screen location===";
        }
        else
        {
            qDebug() << "triggerAction";

            code = "triggerAction(";
            code += QString::number(j); //event types
            code += ", ";
            code += QString::number(index); //index
            code += ")";

            result = browser->JStoString(code);
        }

        if (positionKnown)
        {
            QApplication::restoreOverrideCursor();
        }

        if (result.isEmpty())
            continue;

        logWidget->push(result);

        if (result.contains("===out of range"))
        {
            *stopFlag = true;
            return false;
        }

        browser->wait(true);

        affordanceLabel = MyString::triggerString(j);
        if (!nodeText.isEmpty())
        {
            affordanceLabel += " '";
            affordanceLabel += MyString::truncate(MyString::flatten(nodeText));
            affordanceLabel += "'";
        }
        else if (!nodeName.isEmpty())
        {
           affordanceLabel += " '";
           affordanceLabel += nodeName;
           affordanceLabel += "'";
        }

        //tbd: differentiate affordanceLabel and actionLabel?
        actionLabel = affordanceLabel;

        arrowType = ARROW_TYPE_INIT;
        switch (j)
        {
        case TRIGGER_CLICK:
            arrowType = ARROW_TYPE_EVENT_CLICK;
            break;
        case TRIGGER_MOUSEDOWN:
            arrowType = ARROW_TYPE_EVENT_MOUSEDOWN;
            break;
        case TRIGGER_MOUSEUP:
            arrowType = ARROW_TYPE_EVENT_MOUSEUP;
            break;
        case TRIGGER_MOUSEOVER:
            arrowType = ARROW_TYPE_EVENT_MOUSEOVER;
            break;
        case TRIGGER_SUBMIT:
            arrowType = ARROW_TYPE_EVENT_SUBMIT;
            break;
        case TRIGGER_ARIA:
            arrowType = ARROW_TYPE_EVENT_ARIA;
            break;
        }

        int arrowFlags = 0;

        arrowFlags |= ARROW_FLAG_EVENT;

        //visibility
        {
            code = "nodeIsVisible(";
            code += QString::number(index);
            code += ")";
            if (browser->JStoInt(code))
                arrowFlags |= ARROW_FLAG_VISIBLE;
        }

        //aria
        {
            code = "nodeIsAria(";
            code += QString::number(index);
            code += ")";
            if (browser->JStoInt(code))
                arrowFlags |= ARROW_FLAG_ARIA;
        }

        recurse(
            data,
            browser->url().toString(),
            affordanceLabel,
            actionLabel,
            stateId,
            arrowType,
            arrowFlags
        );
        browser->navigate(origin);
    }
    return true;
}

bool Modeler::handleHyperlink(Data *data, int index, const QString &url, int stateId, int arrowType, const QString &origin)
{
    if (*stopFlag)
    {
        return false;
    }

    QString code, linkText, linkHref, linkHrefAbsolute, affordanceLabel, actionLabel;

    code = "linkText(";
    code += QString::number(index);
    code += ")";
    linkText = browser->JStoString(code);

    code = "linkHref(";
    code += QString::number(index);
    code += ")";
    linkHref = browser->JStoString(code);

    code = "linkHrefAbsolute(";
    code += QString::number(index);
    code += ")";
    linkHrefAbsolute = browser->JStoString(code);

    //skip conditions
    if (
            linkHref.isEmpty() ||
            linkHref == "\"\"" ||
            //blacklist->contains(linkHrefAbsolute) ||
            (!listCheck(linkHrefAbsolute)) ||
            linkHref.contains("mailto:") ||
            linkHrefAbsolute.contains("https:")
       )
    {
        return true;
    }

    //use linkText if available
    if (!linkText.isEmpty())
    {
        affordanceLabel += " '";
        affordanceLabel += MyString::truncate(MyString::flatten(linkText));
        affordanceLabel += "'";
    }
    else //otherwise, truncate appropriate portion of URL
    {
        affordanceLabel = MyString::truncate(MyString::flatten(linkLabel(url,linkHref)));
    }

    //tbd: differentiate actionLabel from affordanceLabel?
    actionLabel = affordanceLabel;

    browser->navigate(linkHrefAbsolute);

    logWidget->push("===set location to " + linkHrefAbsolute + "===\n");

    browser->wait();

    arrowType = ARROW_TYPE_INIT;
    switch (linkType(url, linkHref))
    {
    case LINK_INTERNAL:
        arrowType = ARROW_TYPE_LINK_FRAGMENT;
        break;
    case LINK_EXTERNAL:
        arrowType = ARROW_TYPE_LINK_EXTERNAL;
        break;
    case LINK_RELATIVE:
    case LINK_SAME_HOST:
        arrowType = ARROW_TYPE_LINK_SAME_HOST;
        break;
    }

    int arrowFlags = 0;
    arrowFlags |= ARROW_FLAG_VISIBLE;
    arrowFlags |= ARROW_FLAG_LINK;

    recurse(
        data,
        browser->url().toString(),
        affordanceLabel,
        actionLabel,
        stateId,
        arrowType,
        arrowFlags
    );

    browser->navigate(origin);
    return true;
}
