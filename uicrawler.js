function nodeList()
{
    var list = [];

    var treeWalker = document.createTreeWalker(
            document.body,
    NodeFilter.SHOW_ELEMENT,
    {
        acceptNode: function(node) {
            return NodeFilter.FILTER_ACCEPT;
        }
    },
    false
    );

    var nodeCount = 0;
    while (treeWalker.nextNode())
    {

        var node = treeWalker.currentNode;
        nodeCount++;

        //skip if not an element
        if (node.nodeType != 1) // ELEMENT_NODE
        {
            continue;
        }

        //skip if no listeners
        var hasListener = 0;

        //JQuery
        var events = $(node).data("events");
        if (
                events != null &&
                (events.click != null ||
                 events.mousedown != null ||
                 events.mouseup != null ||
                 events.mouseover != null ||
                 events.submit != null)
           )
        {
            hasListener = 1;
        }

        //on... listeners
        if (
            node.onclick != null ||
            node.onmousedown != null ||
            node.onmouseup != null ||
            node.onmouseover != null ||
            node.onsubmit != null
        )
        {
            hasListener = 1;
        }

        //aria-label elements
        if (node.getAttribute("aria-label") != null ||
            node.getAttribute("aria-labelledby") != null)
        {
            hasListener = 1;
        }

        if (hasListener == 0)
        {
            continue;
        }

        list.push(node);
    }

    return list;
}

function nodeIsAria(intVal)
{
    var i = parseInt(intVal);
    var nodes = nodeList();
    if (i >= nodes.length)
        return '';

    var node = nodes[i];
    return (node.getAttribute("aria-label") != null ||
            node.getAttribute("aria-labelledby") != null);
}

function nodeIsVisible(intVal)
{
    var i = parseInt(intVal);
    var nodes = nodeList();
    if (i >= nodes.length)
        return '';

    var node = nodes[i];

    return (
            node.style.display != 'none' &&
            node.style.visibility != 'hidden' //tbd: add JQuery visibility selectors
    );
}

function nodeText(intVal)
{
    var i = parseInt(intVal);
    var nodes = nodeList();
    if (i >= nodes.length)
        return '';

    var node = nodes[i];

    if (node.getAttribute("aria-label") != null)
    {
        return node.getAttribute("aria-label");
    }

    if (node.getAttribute("aria-labelledby") != null)
    {
        return node.getAttribute("aria-labelledby");
    }

    return nodes[i].textContent;
}

function nodeName(intVal)
{
    var i = parseInt(intVal);
    var nodes = nodeList();
    if (i >= nodes.length)
        return '';
    return nodes[i].nodeName;
}

function linkText(intVal)
{
    var i = parseInt(intVal);
    var links = hyperlinkList();
    if (i >= links.length)
        return '';
    return links[i].textContent;
}

function linkHref(intVal)
{
    var i = parseInt(intVal);
    var links = hyperlinkList();
    if (i >= links.length)
        return '';
    return links[i].getAttribute('href');
}

function linkHrefAbsolute(intVal)
{
    var i = parseInt(intVal);
    var links = hyperlinkList();
    if (i >= links.length)
        return '';
    return links[i].href;
}

function stats()
{
    var str = "===stats===\nnodes with listeners: ";
    str += nodeList().length;
    str += "\nhyperlinks: ";
    str += hyperlinkList().length;
    str += "\n===stats end===\n";
    return str;
}

function nodeCount()
{
    return nodeList().length;
}

function hyperlinkCount()
{
    return hyperlinkList().length;
}

function hyperlinkList()
{
    var list = [];

    var anchors = document.getElementsByTagName('a');
    for (var i = 0; i < anchors.length; i++)
    {
        var anchor = anchors[i];

        var href = anchor.getAttribute('href');

        //check href present
        if (href == null)
            continue;

        list.push(anchor);
    }
    return list;
}

function linkIsVisible(intVal)
{
    var i = parseInt(intVal);
    var hyperlinks = hyperlinkList();
    var count = hyperlinks.length;

    if (intVal >= count)
    {
        return 0;
    }

    var link = links[i];

    return (
            link.style.display != 'none' &&
            link.style.visibility != 'hidden' //tbd: add JQuery visibility selectors
    );
}

function linkIsAria(intVal)
{
    var i = parseInt(intVal);
    var hyperlinks = hyperlinkList();
    var count = hyperlinks.length;

    if (intVal >= count)
    {
        return 0;
    }

    var link = links[i];

    return (link.getAttribute("aria-label") != null ||
            link.getAttribute("aria-labelledby") != null);
}

function linkText(intVal)
{
    var i = parseInt(intVal);
    var links = hyperlinkList();
    if (i >= links.length)
        return '';
    return links[i].textContent;
}

function state()
{
    var s;

    s = "===state===\n";
    s += "title ";
    s += document.title;
    s += "\n";

    var listenerCount = 0;

    var nodes = nodeList();
    var nodeCount = nodes.length;

    for (var i = 0; i < nodeCount; i++)
    {
        var node = nodes[i];
        if (node.onclick != null)
        {
            listenerCount++;
            s += "element #";
            s += i + 1;
            s += " ";
            s += node.localName;
            s += " onclick => ";
            var str = String(node.onclick)
            s += str.replace(/\n/g, " ");
            s += "\n";
        }

        if (node.onmouseover != null)
        {
            listenerCount++;
            s += "element #";
            s += i + 1;
            s += " ";
            s += node.localName;
            s += " onmouseover => ";
            var str = String(node.onmouseover)
            s += str.replace(/\n/g, " ");
            s += "\n";
        }

        if (node.onsubmit != null)
        {
            listenerCount++;
            s += "element #";
            s += i + 1;
            s += " ";
            s += node.localName;
            s += " onsubmit => ";
            var str = String(node.onsubmit)
            s += str.replace(/\n/g, " ");
            s += "\n";
        }

        if (node.getAttribute("aria-label") != null)
        {
            ariaLabel = node.getAttribute("aria-label");
            listenerCount++;
            s += "element #";
            s += i + 1;
            s += " ";
            s += node.localName;
            s += " aria-label: ";
            s += ariaLabel;
            s += "\n";
        }

        if (node.getAttribute("aria-labelledby") != null)
        {
            ariaLabel = node.getAttribute("aria-labelledby");
            listenerCount++;
            s += "element #";
            s += i + 1;
            s += " ";
            s += node.localName;
            s += " aria-labelledby: ";
            s += ariaLabel;
            s += "\n";
        }

    }

    if (listenerCount == 0)
    {
        s += "no listeners found\n";
    }

    //links
    var hyperlinks = hyperlinkList();
    var hyperlinksCount = hyperlinks.length;
    if (hyperlinksCount == 0)
    {
        s += "no hyperlinks found\n";
    }
    else
    {
        for (var i = 0; i < hyperlinksCount; i++)
        {
            var hyperlink = hyperlinks[i];
            var href = hyperlink.getAttribute('href');
            s += "hyperlink #";
            s += i + 1;
            s += " ";
            s += hyperlink.textContent;
            s += " => ";
            s += href;
            s += "\n";
        }
    }

    s += "===state end===";

    //clean up white space
    s = s.replace(/[ \t]{2,}/g, " ");

    return s;
}

function nodePosition(intValIndex)
{
    var index = parseInt(intValIndex);
    var nodes = nodeList();
    var count = nodes.length;

    if (index >= count)
    {
        return "!indexoutofbounds";
    }

    var node = nodes[index];

    if (node == null)
    {
        return "!node==null";
    }

    var s = node.offsetLeft + ":" + node.offsetTop;
    return s;
}

function triggerAction(intValListener, intValIndex)
{
    var listener = parseInt(intValListener);
    var index = parseInt(intValIndex);
    var nodes = nodeList();
    var count = nodes.length;

    if (index >= count)
    {
        var s = "===out of range error (action #";
        s += index + 1;
        s += ") ===\n";
        return s;
    }

    var node = nodes[index];

    var hasClickListener = 0;
    var hasMousedownListener = 0;
    var hasMouseupListener = 0;
    var hasMouseoverListener = 0;
    var hasSubmitListener = 0;
    var hasAriaListener = 0;

    //check for JQuery event listeners
    var events = $(node).data("events");
    if (events != null)
    {
        if (events.click != null)
        {
            hasClickListener = 1;
        }

        if (events.mousedown != null)
        {
            hasMousedownListener = 1;
        }

        if (events.mouseup != null)
        {
            hasMouseupListener = 1;
        }

        if (events.mouseover != null)
        {
            hasMouseoverListener = 1;
        }

        if (events.submit != null)
        {
            hasSubmitListener = 1;
        }
    }

    //check for HTML attribute event listeners
    if (listener == Listener.OnClick && node.onclick != null)
        hasClickListener = 1;

    if (listener == Listener.OnMousedown && node.onmousedown != null)
        hasMousedownListener = 1;

    if (listener == Listener.OnMouseup && node.onmouseup != null)
        hasMouseupListener = 1;

    if (listener == Listener.OnMouseover && node.onmouseover != null)
        hasMouseoverListener = 1;

    if (listener == Listener.OnSubmit && node.onsubmit != null)
        hasSubmitListener = 1;

    if (listener == Listener.OnAria &&
            (node.getAttribute("aria-label") != null ||
             node.getAttribute("aria-labelledby") != null))
    {
        hasAriaListener = 1;
    }

    //only dispatch if both conditions met
    if (listener == Listener.OnClick && hasClickListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("click", true, true);
        node.dispatchEvent(event);
        return "===fired click event===\n";
    }
    else if (listener == Listener.OnMousedown && hasMousedownListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("mousedown", true, true);
        node.dispatchEvent(event);
        return "===fired mousedown event===\n";
    }
    else if (listener == Listener.OnMouseup && hasMouseupListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("mouseup", true, true);
        node.dispatchEvent(event);
        return "===fired mouseup event===\n";
    }
    else if (listener == Listener.OnMouseover && hasMouseoverListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("mouseover", true, true);
        node.dispatchEvent(event);
        return "===fired mouseover event===\n";
    }
    else if (listener == Listener.OnSubmit && hasSubmitListener == 1)
    {
        var event = document.createEvent("HTMLEvents");
        event.initEvent("submit", true, true);
        node.dispatchEvent(event);
        return "===fired submit event===\n";
    }
    else if (listener == Listener.OnAria && hasAriaListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("click", true, true);
        node.dispatchEvent(event);
        return "===fired click event (ARIA)===\n";
    }

    return '';
}

function followLink(intVal)
{
    var i = parseInt(intVal);
    var hyperlinks = hyperlinkList();
    var count = hyperlinks.length;

    if (intVal >= count)
    {
        var s = "===link ";
        s += i + 1;
        s += " out of range===\n";
        return s;
    }

    var href = hyperlinks[intVal].getAttribute("href");
    if (href != null && href[0] == '#')
    {
        var s = "===skipped internal link ";
        s += href;
        s += "===\n";
        return s;
    }

    //node.href forces absolute; see http://groups.google.com/group/comp.lang.javascript/browse_thread/thread/6937160715587627?pli=1
    var hrefAbsolute = hyperlinks[intVal].href;

    if (hrefAbsolute != null)
    {
        window.location = hrefAbsolute;
        var s = "===set location to ";
        s += hrefAbsolute;
        s += "===\n";
        return s;
    }

    return "===href invalid===\n";
}

//see http://code.google.com/p/js-uri/
function authorityFromUri(str)
{
    // Based on the regex in RFC2396 Appendix B.
    var parser = /^(?:([^:\/?\#]+):)?(?:\/\/([^\/?\#]*))?([^?\#]*)(?:\?([^\#]*))?(?:\#(.*))?/;
    var result = str.match(parser);
    
    if (result[2] == null)
    	return '';
    return result[2];
}

var Listener =
{
    OnClick:0,
    OnMousedown:1,
    onMouseup:2,
    OnMouseover:3,
    OnSubmit:4,
    OnAria:5
}

//http://werxltd.com/wp/2010/05/13/javascript-implementation-of-javas-string-hashcode-method/
function hashCode(str)
{
    if (str == null)
        return 0;

    var hash = 0;
    var length = str.length;

    for (i = 0; i < length; i++)
    {
        char = str[i];
        hash = 31 * hash + char;
        hash = hash & hash; // Convert to 32bit integer
    }

    return hash;
}

var Profile =
{
    Default:0,
    Wai:1,
    Events:2,
    Links:3
}
