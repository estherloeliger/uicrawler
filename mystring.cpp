#include "mystring.h"
#include "constants.h"
#include "arrow.h"
#include "state.h"
#include "data.h"
#include <QtGui>

MyString::MyString()
{

}

QString MyString::process(const QString &s)
{
    return MyString::truncate(MyString::flatten(s));
}

QString MyString::flatten(const QString &s)
{
    QString t = s;
    t = t.replace("\n", " ");
    t = t.replace("\r", " ");
    t = t.replace("\t", " ");
    t = t.replace(" {2,}", " ");
    return t;
}

QString MyString::truncate(const QString &s)
{
    if (s.length() < 24)
        return s;
    return s.left(20) + "...";
}

QString MyString::makeState(int i, const QString &title)
{
    QString s = "state ";
    s += QString::number(i);
    s += "\\n";
    s += title;
    return s;
}

QString MyString::triggerString(int trigger)
{
    switch (trigger)
    {
    case TRIGGER_CLICK:
        return "click";
        break;
    case TRIGGER_MOUSEDOWN:
        return "mousedown";
        break;
    case TRIGGER_MOUSEUP:
        return "mouseup";
        break;
    case TRIGGER_MOUSEOVER:
        return "mouseover";
        break;
    case TRIGGER_SUBMIT:
        return "submit";
        break;
    case TRIGGER_ARIA:
        return "aria";
        break;
    case TRIGGER_LINK:
        return "link";
        break;
    case TRIGGER_INIT:
        return "init";
        break;
    }
    return "invalid";
}

QString MyString::statesToMapString(const State &a, const State &b)
{
    QString s;
    s += "(\"";
    s += QString::number(a.id);
    s += "[label=\\\"";
    s += a.title;
    s += "\\\"]\",\"";
    s += QString::number(b.id);
    s += "[label=\\\"";
    s += b.title;
    s += "\\\"]\")";

    return s;
}

QString MyString::arrowsToMapString(const Arrow &a, const Arrow &b)
{
    QString s;
    s += "(\"";
    s += QString::number(a.source);
    s += "->";
    s += QString::number(a.target);
    s += "[label=\\\"";
    s += a.label;
    s += "\\\"]\",\"";
    s += QString::number(b.source);
    s += "->";
    s += QString::number(b.target);
    s += "[label=\\\"";
    s += b.label;
    s += "\\\"]\")";

    return s;
}

QString MyString::arrowVectorToPullbackString(QVector<Arrow> &v)
{
    int count = v.count();

    QStringList edges;
    for (int i = 0; i < count; i++)
    {
        QString edge;
        Arrow a = v.at(i);
        edge += QString::number(a.source);
        edge += "->";
        edge += QString::number(a.target);
        edge += "[label=\\\"";
        edge += a.label;
        edge += "\\\"]";

        edges.append(edge);

        //tbd: populate states
    }

    QString s;
    s = "[";

    int edgeCount = edges.count();
    for (int i = 0; i < edgeCount; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += "\"";
        s += edges.at(i);
        s += "\"";
    }

    s += "]\n";

    return s;
}

QString MyString::stateVectorToPullbackString(QVector<State> &v)
{
    int count = v.count();

    QStringList nodes;
    for (int i = 0; i < count; i++)
    {
        QString node;
        State s = v.at(i);
        node += QString::number(s.id);
        node += "[label=\\\"";
        node += s.title;
        node += "\\\"]";
        nodes.append(node);
    }

    QString s;
    s = "[";

    int nodeCount = nodes.count();
    for (int i = 0; i < nodeCount; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += "\"";
        s += nodes.at(i);
        s += "\"";
    }

    s += "]\n";

    return s;
}

QString MyString::stateMappingsToPullbackString(const QVector<QPair<State, State> > &v)
{
    QString s;

    s = "[";

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += statesToMapString(v.at(i).first, v.at(i).second);
    }

    s += "]";

    return s;
}

QString MyString::edgeMappingsToPullbackString(const QVector<QPair<Arrow, Arrow> > &v)
{
    QString s;

    s = "[";

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += arrowsToMapString(v.at(i).first, v.at(i).second);
    }

    s += "]";

    return s;
}

QString MyString::dataToDotString(Data *data, int type)
{
    QString s;

    //select edges/titles based on type
    QVector<Arrow> *edgesPtr;
    QHash<int, QString> *titlesPtr;
    QString filter;

    switch (type)
    {
    case DOT_TYPE_AFFORDANCE:
        titlesPtr = &(data->affordanceStateTitles);
        edgesPtr = &(data->affordanceEdges);
        filter = data->affordanceFilter;
        break;
    case DOT_TYPE_ACTION:
        titlesPtr = &(data->actionStateTitles);
        edgesPtr = &(data->actionEdges);
        filter = data->actionFilter;
        break;
    case DOT_TYPE_ABSTRACT:
        titlesPtr = &(data->abstractStateTitles);
        edgesPtr = &(data->abstractEdges);
        break;
    default:
        return s;
        break;
    }

    //parse filter string
    QStringList stateNos = filter.split(",");
    QSet<int> filteredSet;
    int filteredSetSize = stateNos.count();
    //build set of filtered states
    for (int index = 0; index < filteredSetSize; index++)
    {
        int value = stateNos.at(index).toInt();
        if (!value)
            continue;
        filteredSet.insert(value);
    }

    s += ("digraph d {\n");
    s += ("graph [ bgcolor=\"white\", resolution=\"128\", fontname=\"Helvetica\", fontcolor=\"black\", fontsize=\"10\" ];");
    s += ("node [ fontname=\"Helvetica\", penwidth=\"0.25\", fontcolor=\"gray32\", fontsize=\"8\"];");
    s += ("edge [ color=\"gray32\", arrowsize=\"0.75\", penwidth=\"0.25\", fontname=\"Helvetica\", fontcolor=\"dodgerblue4\", fontsize=\"8\", arrowhead=\"vee\" ];");

    //states
    std::map<int, QString> states; //tbd: check if necessary
    int size = edgesPtr->count();
    for (int i = 1; i < size; i++) //omit initial state 0
    {
        int sourceId, targetId;
        sourceId = edgesPtr->at(i).source;
        targetId = edgesPtr->at(i).target;
        QString source, target;
        source = MyString::makeState(sourceId, (*titlesPtr)[sourceId]);
        target = MyString::makeState(targetId, (*titlesPtr)[targetId]);
        states.insert(std::make_pair(sourceId, source));
        states.insert(std::make_pair(targetId, target));
    }

    std::map<int, QString>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        bool active = true;
        if (!filter.isEmpty() && !filteredSet.contains(it->first))
        {
            active = false;
        }

        QString state;

        if (!active)
        {
            state += "//";
        }
        state += QString::number(it->first);
        state += " [label=\"";
        state += it->second;
        state += "\"]\n";

        s += state;
    }

    //arrows
    size = edgesPtr->count();
    int source, target;

    for (int i = 1; i < size; i++) //omit initial state 0
    {
        Arrow a = edgesPtr->at(i);

        source = a.source;
        target = a.target;

        bool active = true;
        if (!filter.isEmpty() &&
            ((!filteredSet.contains(source) || !filteredSet.contains(target))))
        {
            active = false;
        }

        QString arrow;

        if (!active)
        {
            arrow += "//";
        }

        arrow += a.toString();

        s += arrow;
    }

    //end dot structure
    s += "}\n";

    return s;
}

QString MyString::dataToPullbackDotString(Data *data)
{
    QString buffer;

    //exp
    buffer += MyString::stateVectorToPullbackString(data->affordanceStates);
    buffer += MyString::arrowVectorToPullbackString(data->affordanceEdges);
    buffer += MyString::stateVectorToPullbackString(data->actionStates);
    buffer += MyString::arrowVectorToPullbackString(data->actionEdges);
    buffer += MyString::stateVectorToPullbackString(data->abstractStates);
    buffer += MyString::arrowVectorToPullbackString(data->abstractEdges);

    buffer += MyString::stateMappingsToPullbackString(data->mapAffordanceToAbstractNodes); //separate collection for states needed?
    buffer += "\n";
    buffer += MyString::edgeMappingsToPullbackString(data->mapAffordanceToAbstractEdges);
    buffer += "\n";
    buffer += MyString::stateMappingsToPullbackString(data->mapActionToAbstractNodes);
    buffer += "\n";
    buffer += MyString::edgeMappingsToPullbackString(data->mapActionToAbstractEdges);
    buffer += "\n";

    //fetch dot file from Haskell routine
    QString tempPath = QDir::tempPath();

    QString serializePath, dotPath;

    for (int i = 0; ; i++)
    {
        serializePath = tempPath;
        serializePath += "/uic";
        serializePath += QString::number(i);
        serializePath += + ".txt";
        if (!QFile::exists(serializePath))
            break;
    }

    for (int j = 0; ; j++)
    {
        dotPath = tempPath;
        dotPath += "/uic";
        dotPath += QString::number(j);
        dotPath += ".dot";
        if (!QFile::exists(dotPath))
            break;
    }

    QFile serializeFile(serializePath);
    serializeFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&serializeFile);
    out << buffer;
    serializeFile.close();

    //qDebug() << "Wrote string:\n" << buffer << "to file: " << serializePath;

#ifdef Q_OS_LINUX
    QProcess::execute("cd /tmp");
#endif

    QString haskellCmd;
    haskellCmd += QCoreApplication::applicationDirPath();
    haskellCmd += "/bin/pullbacks_first ";
    haskellCmd += serializePath;
    haskellCmd += " ";
    haskellCmd += dotPath;

    qDebug() << haskellCmd;
    int ret = QProcess::execute(haskellCmd);

    if (ret)
    {
        qDebug() << "Haskell command returned error value";
        qDebug() << "Wrote string:\n" << buffer << "to file: " << serializePath;
        return "";
    }

    //read dot file
    QString dot;

    QFile file;
    file.setFileName(dotPath);
    file.open(QIODevice::ReadOnly);
    dot = file.readAll();
    file.close();

    return dot;
}

