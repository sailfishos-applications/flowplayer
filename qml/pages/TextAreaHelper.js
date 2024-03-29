function findFlickable(component) {
    var nextParent = component
    var flickableItem = null
    while(nextParent) {
        if(nextParent.flicking !== undefined && nextParent.flickableDirection !== undefined)
            flickableItem = nextParent

        nextParent = nextParent.parent
    }
    if (flickableItem) return flickableItem
    return null
}

function animateContentY(animation, flickable, newContentY) {
    animation.target = flickable
    animation.to = newContentY
    animation.running = true
}

function locateFlickableY(flickable) {
    switch(screen.currentOrientation) {
    case Screen.Landscape:
        return flickable.mapToItem(null, flickable.x, flickable.y).y

    case Screen.LandscapeInverted:
        return screen.displayHeight - flickable.mapToItem(null, flickable.x, flickable.y).y

    case Screen.Portrait:
        return flickable.mapToItem(null, flickable.x, flickable.y).x

    case Screen.PortraitInverted:
        return screen.displayWidth - flickable.mapToItem(null, flickable.x, flickable.y).x
    }
}

function getMargin() {
    switch(screen.currentOrientation) {
    case Screen.Landscape:
    case Screen.LandscapeInverted:
        return 40
    case Screen.Portrait:
    case Screen.PortraitInverted:
        return 48
    }

    return 0
}

function repositionFlickable(animation) {
    inputContext.updateMicroFocus()
    var mf = inputContext.microFocus

    if(mf.x == -1 && mf.y == -1)
        return

    var object = findFlickable(parent)

    if(object){
        var flickable = object

        // Specifies area from bottom and top when repositioning should be triggered
        var margin = getMargin()
        var newContentY = flickable.contentY
        var flickableY = locateFlickableY(flickable)

        switch(screen.currentOrientation) {
        case Screen.Landscape:
            if(flickableY + flickable.height  - mf.height - margin < mf.y) {
                // Find dY just to make textfield visible
                var dY = mf.y - flickableY - flickable.height
                // Center textfield
                dY += flickable.height / 2
                newContentY += dY
            } else if(flickableY + margin > mf.y) {
                var dY = flickableY - mf.y
                dY += flickable.height / 2
                newContentY -= dY
            }

            break

        case Screen.LandscapeInverted:
            // In inverted screen we need to compensate for the focus height
            var invertedMfY = screen.displayHeight - mf.y - mf.height

            if(flickableY + flickable.height - mf.height - margin < invertedMfY) {
                var dY = invertedMfY - flickableY - flickable.height
                dY += flickable.height / 2 + mf.height / 2
            } else if(flickableY + margin > invertedMfY){
                var dY = flickableY - invertedMfY
                dY += flickable.height / 2 - mf.height / 2
                newContentY -= dY
            }

            break

        case Screen.Portrait:
            if(flickableY + flickable.height - mf.width - margin < mf.x) {
                var dY = mf.x - flickableY - flickable.height
                dY += flickable.height / 2
                newContentY += dY
            } else if(flickableY + margin > mf.x){
                var dY = flickableY - mf.x
                dY += flickable.height / 2
                newContentY -= dY
            }

            break

        case Screen.PortraitInverted:
            var invertedMfX = screen.displayWidth - mf.x - mf.width

            if(flickableY + flickable.height - mf.width - margin < invertedMfX) {
                var dY = invertedMfX - flickableY - flickable.height + mf.height
                dY += flickable.height / 2 + mf.height
                newContentY += dY
            } else if(flickableY + margin > invertedMfX){
                var dY = flickableY - invertedMfX
                dY += flickable.height / 2 - mf.height
                newContentY -= dY
            }

            break
        }

        // If overpanned, set contentY to max possible value (reached bottom)
        if(newContentY > flickable.contentHeight - flickable.height)
            newContentY = flickable.contentHeight - flickable.height

        // If overpanned, set contentY to min possible value (reached top)
        if(newContentY < 0)
            newContentY = 0

        if(newContentY != flickable.contentY) {
            animateContentY(animation, flickable, newContentY)
        }
    }
}

function injectWordToPreedit(newCursorPosition, text) {
    var preeditStart = previousWordStart(newCursorPosition, text);
    var preeditEnd = nextWordEnd(newCursorPosition, text);

    // copy word to preedit text
    var preeditText = getRichText(text).slice(preeditStart, preeditEnd).join("")
    if (preeditText.indexOf("<img") != -1)
        return

    // inject preedit
    cursorPosition = preeditStart;

    var eventCursorPosition = newCursorPosition-preeditStart;

    return inputContext.setPreeditText(preeditText, eventCursorPosition, 0, preeditText.length);
}

function previousWordStart(pos, text) {
    var ret = pos;

    if (ret && atWordSeparator(ret - 1, text)) {
        ret--;
        while (ret && atWordSeparator(ret - 1, text))
            ret--;
    } else {
        while (ret && !atSpace(ret - 1, text) && !atWordSeparator(ret - 1, text))
            ret--;
    }

    return ret;
}

function nextWordEnd(pos, text) {
    var ret = pos;
    var len = getRichText(text).length;

    if (ret < len && atWordSeparator(ret, text)) {
        ret++;
        while (ret < len && atWordSeparator(ret, text))
            ret++;
    } else {
        while (ret < len && !atSpace(ret, text) && !atWordSeparator(ret, text))
            ret++;
    }

    return ret;
}

function getRichText(text) {
	var richText = text.split("</head>")[1].split("</body>")[0]
	if (richText.indexOf("-qt-paragraph-type:empty;") != -1)
	    richText = richText.replace("text-indent:0px;\"><br />","text-indent:0px;\">")
	richText = richText.replace(/\<p[^\>]*\>/g, "").replace(/<\/p>/g, "")
	richText = richText.replace(/\<body[^\>]*\>\n/g, "")
	richText = richText.replace(/\n/g, "<br />")
	
	var listText = []
	for(var i =0; i<richText.length; i++)
	{
	    if (richText[i] == "<")
	    {
		var j =  richText.indexOf(">", i+1)
		listText.push(richText.substring(i, j+1))
		i = j
	    }
	    else if (richText[i] == "&")
	    {
		var j =  richText.indexOf(";", i+1)
		listText.push(richText.substring(i, j+1))
		i = j
	    }
	    else
		listText.push(richText[i])
	}
	return listText
}

function atSpace(pos, text) {
    var c = getRichText(text)[pos];
    return c == ' '
           || c == '\t'
           || c == '\n'
           || c == '<br />'
           || c == '&nbsp;'
           ;
}

function atWordSeparator(pos, text) {
    switch (getRichText(text)[pos]) {
    case '.':
    case ',':
    case '?':
    case '!':
    case '@':
    case '#':
    case '$':
    case ':':
    case ';':
    case '-':
    case '<':
    case '&lt;':
    case '>':
    case '&gt;':
    case '[':
    case ']':
    case '(':
    case ')':
    case '{':
    case '}':
    case '=':
    case '/':
    case '+':
    case '%':
    case '&':
    case '&amp;':
    case '^':
    case '*':
    case '\'':
    case '"':
    case '&quot;':
    case '`':
    case '~':
    case '|':
        return true;
    default:
        return false;
    }
}

var MIN_UPDATE_INTERVAL = 30
var lastUpdateTime
function filteredInputContextUpdate() {
    if (Date.now() - lastUpdateTime > MIN_UPDATE_INTERVAL || !lastUpdateTime) {
        inputContext.update();
        lastUpdateTime = Date.now();
    }
}
