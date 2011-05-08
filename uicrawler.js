function nodeList(intProfile)
{
    var profile = parseInt(intProfile);

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

        //skip if invisible
        if (
                node.style.display == 'none' ||
                node.style.visibility == 'hidden'
        )
        {
            continue;
        }

        //skip if no listeners
        var hasListener = 0;

        var events = $(node).data("events");
        if (
                events != null &&
                (events.click != null ||
                 events.mouseover != null ||
                 events.submit != null)
           )
        {
            hasListener = 1;
        }

        if (
            node.onclick != null ||
            node.onmouseover != null ||
            node.onsubmit != null
        )
        {
            hasListener = 1;
        }

        if (hasListener == 0)
        {
            continue;
        }

        //skip if PROFILE_FINE_MOTOR and below cutoff
        if (profile == 1 && (node.width < 100 || node.height < 100))
        {
            continue;
        }

        list.push(node);
    }

    return list;
}

function nodeText(intVal, intProfile)
{
    var i = parseInt(intVal);
    var profile = parseInt(intProfile);
    var nodes = nodeList(profile);
    if (i >= nodes.length)
        return '';
    return nodes[i].textContent;
}

function nodeName(intVal, intProfile)
{
    var i = parseInt(intVal);
    var profile = parseInt(intProfile);
    var nodes = nodeList(profile);
    if (i >= nodes.length)
        return '';
    return nodes[i].nodeName;
}

function linkText(intVal, intProfile)
{
    var i = parseInt(intVal);
    var profile = parseInt(intProfile);
    var links = hyperlinkList(profile);
    if (i >= links.length)
        return '';
    return links[i].textContent;
}

function linkHref(intVal, intProfile)
{
    var i = parseInt(intVal);
    var profile = parseInt(intProfile);
    var links = hyperlinkList(profile);
    if (i >= links.length)
        return '';
    return links[i].getAttribute('href');
}

function stats(intProfile)
{
    var profile = parseInt(intProfile);
    var str = "===stats===\nnodes with listeners: ";
    str += nodeList(profile).length;
    str += "\nhyperlinks: ";
    str += hyperlinkList(profile).length;
    str += "\n===stats end===\n";
    return str;
}

function nodeCount(intProfile)
{
    var profile = parseInt(intProfile);
    return nodeList(profile).length;
}

function hyperlinkCount(intProfile)
{
    var profile = parseInt(intProfile);
    return hyperlinkList(profile).length;
}

function hyperlinkList(intProfile)
{
    var profile = parseInt(intProfile);

    var list = [];

    var anchors = document.getElementsByTagName('a');
    for (var i = 0; i < anchors.length; i++)
    {
        var anchor = anchors[i];

        //skip if invisible
        if (
                anchor.style.display == 'none' ||
                anchor.style.visibility == 'hidden'
        )
        {
            continue;
        }

        var href = anchor.getAttribute('href');

        //check href present
        if (href == null)
            continue;

        if (profile == 1 && (anchor.width < 100 || anchor.height < 100))
            continue;

        list.push(anchor);
    }
    return list;
}

function state(intProfile)
{
    var profile = parseInt(intProfile);
    var s;

    s = "===state===\n";
    s += "title ";
    s += document.title;
    s += "\n";

    var listenerCount = 0;

    var nodes = nodeList(profile);
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
    }

    if (listenerCount == 0)
    {
        s += "no listeners found\n";
    }

    //links
    var hyperlinks = hyperlinkList(profile);
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

function triggerAction(intValListener, intValIndex, intProfile)
{
    var listener = parseInt(intValListener);
    var index = parseInt(intValIndex);
    var profile = parseInt(intProfile);
    var nodes = nodeList(profile);
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
    var hasMouseoverListener = 0;
    var hasSubmitListener = 0;

    //check for JQuery event listeners
    var events = $(node).data("events");
    if (events != null)
    {
        if (events.click != null)
        {
            hasClickListener = 1;
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

    if (listener == Listener.OnMouseover && node.onmouseover != null)
        hasMouseoverListener = 1;

    if (listener == Listener.OnSubmit && node.onsubmit != null)
        hasSubmitListener = 1;

    if (listener == Listener.OnClick && hasClickListener == 1)
    {
        var event = document.createEvent("MouseEvents");
        event.initEvent("click", true, true);
        node.dispatchEvent(event);
        return "===fired click event===\n";
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

    return '';
}

function followLink(intVal, intProfile)
{
    var i = parseInt(intVal);
    var profile = parseInt(intProfile);
    var hyperlinks = hyperlinkList(profile);
    var count = hyperlinks.length;

    if (intVal >= count)
    {
        var s = "===link ";
        s += i + 1;
        s += " out of range===\n";
        return s;
    }

    var href = hyperlinks[intVal].getAttribute('href');

    if (href != null)
    {
        window.location = href;
        var s = "===set location to ";
        s += href;
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
    OnMouseover:1,
    OnSubmit:2
}