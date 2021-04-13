import QtQuick 1.1
import com.nokia.meego 1.0
import FlowPlayer 1.0


Page {
    id: root

    orientationLock: utils.orientation=="auto" ? PageOrientation.Automatic :
                     utils.orientation=="portrait" ? PageOrientation.LockPortrait :
                     PageOrientation.LockLandscape

    property bool artistReaded
    property bool albumReaded
    property bool songReaded
    property string defaultAlbumArt

    property bool loginIn: false


    CheckBoxStyle {
        id: myCheckBoxStyle
        background: currentTheme==="sailfish"? "qrc:/images/switch-off.png" : "image://theme/meegotouch-button-checkbox-inverted-background"
        backgroundSelected: currentTheme==="sailfish"? "qrc:/images/switch-on.png" : "image://theme/color12-meegotouch-button-checkbox-inverted-background-selected"
        backgroundPressed: currentTheme==="sailfish"? "qrc:/images/switch-on.png" : "image://theme/color12-meegotouch-button-checkbox-inverted-background-pressed"
        backgroundDisabled: currentTheme==="sailfish"? "qrc:/images/switch-off.png" : "image://theme/meegotouch-button-checkbox-inverted-background-disabled"
    }


    tools: ToolBarLayout {
        id: toolBar

        ToolIcon {
            platformStyle: ToolItemStyle{ pressedBackground: currentTheme==="sailfish"? "": "image://theme/meegotouch-button-navigationbar-button-inverted-background-pressed" }
            iconSource: currentTheme==="sailfish"? "qrc:/images/toolbar-back.png" : "image://theme/icon-m-toolbar-back-white"
            onClicked: pageStack.pop()
        }

        ToolIcon
        {
            iconSource: currentTheme==="sailfish"? "qrc:/images/sailfish-artist.png" : "qrc:/images/toolbar-artist.png"
            onClicked: {
                rectArtist.visible = true
                rectAlbum.visible = false
                rectSong.visible = false
                lastfmSetup.visible = false
            }
        }

        ToolIcon
        {
            iconSource: currentTheme==="sailfish"? "qrc:/images/sailfish-cd.png" : "qrc:/images/toolbar-cd.png"
            onClicked: {
                rectArtist.visible = false
                rectAlbum.visible = true
                rectSong.visible = false
                lastfmSetup.visible = false
            }
        }

        ToolIcon
        {
            iconSource: currentTheme==="sailfish"? "qrc:/images/toolbar-audio.png" : "qrc:/images/audio.png"
            onClicked: {
                rectArtist.visible = false
                rectAlbum.visible = false
                rectSong.visible = true
                lastfmSetup.visible = false
            }
        }


        ToolIcon {
            iconSource: currentTheme==="sailfish"? "qrc:/images/toolbar-settings.png" : "image://theme/icon-m-toolbar-settings-white"
            onClicked: {
                rectArtist.visible = false
                rectAlbum.visible = false
                rectSong.visible = false
                lastfmSetup.visible = true
            }
        }
    }

    TextFieldStyle {
        id: myTextFieldStyle
        background: currentTheme==="sailfish"? "file:///usr/share/themes/sailfish/meegotouch/images/search/inputfieldlandscape.png" :
                                 "image://theme/meegotouch-textedit-background"
        backgroundSelected: currentTheme==="sailfish"? "file:///usr/share/themes/sailfish/meegotouch/images/search/inputfieldlandscape.png" :
                                         "image://theme/color12-meegotouch-textedit-background-selected"
        selectionColor: themeColor
        textColor: currentTheme==="sailfish"? "white" : "black"
    }

    Component.onCompleted:
    {
        readData()
    }

    function readData()
    {
        if ( lastFMartist != currentSongArtist)
        {
            bioText.text= qsTr("Fetching artist information")
            lfm.getBio(currentSongArtist)
            lastFMartist = currentSongArtist
            artistReaded = false
        }
        else
        {
            bioText.text = ""
            bioText.text = lfm.bioInfo()
            thumb.source = lfm.bioImg()
            artistReaded = true
        }

        if ( lastFMalbum != currentSongAlbum)
        {
            bioText2.text= qsTr("Fetching album information")
            lfm.getAlbumBio(currentSongArtist, currentSongAlbum)
            lastFMalbum = currentSongAlbum
            albumReaded = false
        }
        else
        {
            bioText2.text = ""
            bioText2.text = lfm.bioInfoAlbum()
            //thumb2.source = utils.bioImgAlbum()
            albumReaded = true
        }

        if ( lastFMsong != currentSongTitle)
        {
            bioText3.text= qsTr("Fetching track information")
            lfm.getSongBio(currentSongArtist, currentSongTitle)
            lastFMsong = currentSongTitle
            songReaded = false
        }
        else
        {
            bioText3.text = ""
            bioText3.text = lfm.bioInfoSong()
            //thumb.source = utils.bioImgSong()
            songReaded = true
        }
    }


    /*Image {
        id: background
        anchors.fill: parent
        source: "image://theme/meegotouch-video-background"
        fillMode: Image.Stretch
    }*/

    Flickable {
        id: rectArtist
        anchors.fill: parent
        contentWidth: width
        contentHeight: column1.height + 20
        visible: true
        clip: true

        Column {
            id: column1
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 0 }
            spacing: 20
            visible: currentSongArtist !== ""
                     && currentSongTitle !== ""

            Item {
                x: 20
                width: parent.width -40
                height: 50

                Image {
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    x: -12
                    source: "qrc:/images/lastfm2.png"
                    smooth: true
                    height: 28
                    width: 120
                    fillMode: Image.PreserveAspectFit
                }

                Image {
                    id: image1
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    height: 32; width: 32; smooth: true
                    source: "image://theme/icon-m-common-refresh"
                    opacity: 0.6
                    visible: bioText.text!=qsTr("Fetching artist information") ? true : false
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bioText.text= qsTr("Fetching artist information")
                            thumb.source = ""
                            lfm.getBio(currentSongArtist)
                        }
                    }
                }

                MyBusyIndicator {
                    id: busyIndicator1
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    implicitWidth: 32
                    visible: !image1.visible
                    running: visible
                }
            }

            Rectangle {
                x: 20
                width: parent.width - 40
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }


            Row {
                x: 20
                width: parent.width -40
                spacing: 14
                Image {
                    height: 32; width: 32; smooth: true
                    source: currentTheme==="sailfish"? "qrc:/images/sailfish-artist.png" : "qrc:/images/toolbar-artist.png"
                }
                Text {
                    font.pixelSize: 26; color: "white"
                    text: currentSongArtist
                    width: parent.width - 50
                    elide: Text.ElideRight
                    onTextChanged: { if (text!=="" ) readData() }
                }
            }

            Image {
                id: thumb
                x: 20
                visible: source!=""
                cache: true
                smooth: true
            }

            Text {
                x: 20
                id: bioText
                //anchors.top: thumb.bottom
                //anchors.topMargin: 20
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: appWindow.inPortrait ? 440 : 814
                font.pixelSize: 20
                color: "white"
            }

            MyButton {
                x: 20
                id: plus
                width: 120
                style: currentTheme==="sailfish"? "flat" : "normal"

                visible: bioText.text == qsTr("Fetching artist information") ||
                         bioText.text == qsTr("No artist information available") ||
                         bioText.text == qsTr("Error fetching artist information") ||
                         bioText.text == qsTr("The artist could not be found") ? false : true

                iconSource: bioText.text==lfm.bioInfo() ?
                                (currentTheme==="blanco"? "image://theme/icon-m-toolbar-down-white" : "qrc:/images/toolbar-down.png") :
                                (currentTheme==="blanco"? "image://theme/icon-m-toolbar-up-white" : "qrc:/images/toolbar-up.png")

                onClicked: {
                    if ( bioText.text==lfm.bioInfo() )
                        bioText.text = lfm.bioInfoLarge()
                    else
                        bioText.text = lfm.bioInfo()
                }
            }

        }
    }


    Flickable {
        id: rectAlbum
        anchors.fill: parent
        contentWidth: width
        contentHeight: column2.height + 20
        visible: false
        clip: true

        Column {
            id: column2
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 0 }
            spacing: 20
            visible: currentSongArtist !== ""
                     && currentSongTitle !== ""

            Item {
                x: 20
                width: parent.width -40
                height: 50

                Image {
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    x: -12
                    source: "qrc:/images/lastfm2.png"
                    smooth: true
                    height: 28
                    width: 120
                    fillMode: Image.PreserveAspectFit
                }

                Image {
                    id: image2
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    height: 32; width: 32; smooth: true
                    source: "image://theme/icon-m-common-refresh"
                    opacity: 0.6
                    visible: bioText2.text!=qsTr("Fetching album information") ? true : false

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bioText2.text= qsTr("Fetching album information")
                            lfm.getAlbumBio(currentSongArtist, currentSongTitle)
                        }
                    }
                }
                MyBusyIndicator {
                    id: busyIndicator2
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    implicitWidth: 32
                    visible: !image2.visible
                    running: visible
                }

            }

            Rectangle {
                x: 20
                width: parent.width - 40
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }


            Row {
                x: 20
                width: parent.width -40
                spacing: 14
                Image {
                    height: 32; width: 32; smooth: true
                    source: currentTheme==="sailfish"? "qrc:/images/sailfish-artist.png" : "qrc:/images/toolbar-artist.png"
                }
                Text {
                    font.pixelSize: 26; color: "white"
                    text: currentSongArtist
                    width: parent.width - 50
                    elide: Text.ElideRight
                }
            }

            Row {
                x: 20
                width: parent.width -40
                spacing: 14
                Image {
                    height: 32; width: 32; smooth: true
                    source: currentTheme==="sailfish"? "qrc:/images/sailfish-cd.png" : "qrc:/images/toolbar-cd.png"
                }
                Text {
                    font.pixelSize: 22; color: "white"; y: 3
                    text: currentSongAlbum
                    width: parent.width - 50
                    elide: Text.ElideRight
                    onTextChanged: { if (text!=="" ) readData() }
                }
            }


            Image {
                id: thumb2
                x: 20
                source: getCover(utils.thumbnail(currentSongArtist, currentSongAlbum))
                fillMode: Image.PreserveAspectFit
                height: 240; width: 240; smooth: true
                cache: true
            }

            Text {
                x: 20
                id: bioText2
                //anchors.top: thumb2.bottom
                //anchors.topMargin: 20
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: appWindow.inPortrait ? 440 : 814
                font.pixelSize: 20
                color: "white"
            }

            MyButton {
                x: 20
                id: plus2
                width: 120
                style: currentTheme==="sailfish"? "flat" : "normal"

                visible: bioText2.text == qsTr("Fetching album information") ||
                         bioText2.text == qsTr("No album information available") ||
                         bioText2.text == qsTr("Error fetching album information") ||
                         bioText2.text == qsTr("The album could not be found") ? false : true

                iconSource: bioText2.text==lfm.bioInfoAlbum() ?
                            (currentTheme==="blanco"? "image://theme/icon-m-toolbar-down-white" : "qrc:/images/toolbar-down.png") :
                            (currentTheme==="blanco"? "image://theme/icon-m-toolbar-up-white" : "qrc:/images/toolbar-up.png")

                onClicked: {
                    if ( bioText2.text==lfm.bioInfoAlbum() )
                        bioText2.text = lfm.bioInfoAlbumLarge()
                    else
                        bioText2.text = lfm.bioInfoAlbum()
                }
            }

        }
    }


    Flickable {
        id: rectSong
        anchors.fill: parent
        contentWidth: width
        contentHeight: column3.height + 20
        visible: false
        clip: true

        Column {
            id: column3
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 0 }
            spacing: 20
            visible: currentSongArtist !== ""
                     && currentSongTitle !== ""

            Item {
                x: 20
                width: parent.width -40
                height: 50

                Image {
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    x: -12
                    source: "qrc:/images/lastfm2.png"
                    smooth: true
                    height: 28
                    width: 120
                    fillMode: Image.PreserveAspectFit
                }

                Image {
                    id: image3
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    height: 32; width: 32; smooth: true
                    source: "image://theme/icon-m-common-refresh"
                    opacity: 0.6
                    visible: bioText3.text!=qsTr("Fetching track information") ? true : false
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bioText3.text= qsTr("Fetching track information")
                            lfm.getSongBio(currentSongArtist, currentSongTitle)
                        }
                    }
                }
                MyBusyIndicator {
                    id: busyIndicator3
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    implicitWidth: 32
                    visible: !image3.visible
                    running: visible
                }

            }

            Rectangle {
                x: 20
                width: parent.width - 40
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }

            Row {
                x: 20
                width: parent.width - 40
                spacing: 14
                Image {
                    height: 32; width: 32; smooth: true
                    source: currentTheme==="sailfish"? "qrc:/images/sailfish-artist.png" : "qrc:/images/toolbar-artist.png"
                }
                Text {
                    font.pixelSize: 26; color: "white"
                    text: currentSongArtist
                    width: parent.width - 50
                    elide: Text.ElideRight
                }
            }
            Row {
                x: 20
                width: parent.width - 40
                spacing: 14
                Image {
                    height: 32; width: 32; smooth: true
                    source: currentTheme==="sailfish"? "qrc:/images/toolbar-audio.png" : "qrc:/images/audio.png"
                }
                Text {
                    font.pixelSize: 22; color: "white"; y: 3
                    text: currentSongTitle
                    width: parent.width - 50
                    elide: Text.ElideRight
                    onTextChanged: { if (text!=="" ) readData() }
                }
            }

            Image {
                x: 20
                id: thumb3
                source: getCover(utils.thumbnail(currentSongArtist, currentSongTitle))
                fillMode: Image.PreserveAspectFit
                height: 240; width: 240; smooth: true
                cache: true
            }

            Text {
                x: 20
                id: bioText3
                //anchors.top: thumb3.bottom
                //anchors.topMargin: 20
                wrapMode: TextEdit.WordWrap
                textFormat: Text.RichText
                width: appWindow.inPortrait ? 440 : 814
                font.pixelSize: 20
                color: "white"
            }

            Row {
                x: 20
                spacing: 12

                MyButton {
                    id: plus3
                    width: 120
                    style: currentTheme==="sailfish"? "flat" : "normal"

                    visible: bioText3.text == qsTr("Fetching track information") ||
                             bioText3.text == qsTr("No track information available") ||
                             bioText3.text == qsTr("Error fetching track information") ||
                             bioText3.text == qsTr("The track could not be found") ? false : true

                    iconSource: bioText3.text==lfm.bioInfoSong() ?
                                (currentTheme==="blanco"? "image://theme/icon-m-toolbar-down-white" : "qrc:/images/toolbar-down.png") :
                                (currentTheme==="blanco"? "image://theme/icon-m-toolbar-up-white" : "qrc:/images/toolbar-up.png")

                    onClicked: {
                        if ( bioText3.text==lfm.bioInfoSong() )
                            bioText3.text = lfm.bioInfoSongLarge()
                        else
                            bioText3.text = lfm.bioInfoSong()
                    }
                }

                MyButton {
                    id: scroble
                    width: 120
                    style: currentTheme==="sailfish"? "flat" : "normal"
                    enabled: !scrobbled

                    visible: bioText3.text == qsTr("Fetching track information") ||
                             bioText3.text == qsTr("Error fetching track information") ||
                             bioText3.text == qsTr("The track could not be found") ? false :
                             lfm.lfmKey() === "" ? false : true

                    iconSource: currentTheme==="sailfish"? "qrc:/images/sailfish-scrobble.png" : "qrc:/images/scrobble.png"

                    onClicked: {
                        lfm.scrobbleSong(currentSongArtist, currentSongAlbum, currentSongTitle)
                        scrobbled = true
                    }
                }

                MyButton {
                    id: love
                    width: 120
                    style: currentTheme==="sailfish"? "flat" : "normal"

                    visible: bioText3.text == qsTr("Fetching track information") ||
                             bioText3.text == qsTr("Error fetching track information") ||
                             bioText3.text == qsTr("The track could not be found") ? false :
                             lfm.lfmKey() === "" ? false : true

                    iconSource: currentTheme==="sailfish"? "qrc:/images/sailfish-love.png" :
                                                           "image://theme/icon-m-toolbar-frequent-used-white"

                    onClicked: {
                        if ( lfm.lfmLove() != "loved" )
                            lfm.loveSong(currentSongArtist, currentSongTitle)
                        else
                            lfm.unloveSong(currentSongArtist, currentSongTitle)
                    }
                }

            }

        }
    }


    Flickable {
        id: lastfmSetup
        anchors.fill: parent
        contentWidth: width
        contentHeight: column4.height + 20
        visible: false
        clip: true

        Column {
            id: column4
            anchors { top: parent.top; left: parent.left; right: parent.right; }
            anchors.leftMargin: 10
            spacing: 20
            visible: currentSongArtist !== ""
                     && currentSongTitle !== ""

            Item {
                x: 10
                width: parent.width -40
                height: 50

                Image {
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    x: -12
                    source: "qrc:/images/lastfm2.png"
                    smooth: true
                    height: 28
                    width: 120
                    fillMode: Image.PreserveAspectFit
                }

                MyBusyIndicator {
                    id: busyIndicator4
                    anchors.top: parent.top
                    anchors.topMargin: 18
                    anchors.right: parent.right
                    implicitWidth: 32
                    visible: loginIn
                    running: visible
                }

            }

            Rectangle {
                x: 10
                width: parent.width - 30
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }


            Label {
                id: logresult
                x: 10
                width: parent.width - 20
                elide: Text.ElideRight
                font.pixelSize: 24
                color: "white"
                text: lfm.lfmKey()==="" ? qsTr("Not logged yet") :
                      lfm.lfmKey()=="error" ? qsTr("Login error. Try again") :
                      qsTr("Logged as ") + lfm.lfmUser()
            }

            Rectangle {
                x: 10
                width: parent.width -30
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }

            Column {
                x: 10
                spacing: 10
                width: parent.width -30
                Label {
                    text: qsTr("User")
                    font.pixelSize: 20
                    color: "white"
                    wrapMode: Text.WordWrap
                }
                TextField {
                    id: userField
                    width: parent.width
                    text: utils.readSettings("lfmUser", "")
                    platformStyle: myTextFieldStyle
                }
            }

            Column {
                x: 10
                spacing: 10
                width: parent.width -30
                Label {
                    text: qsTr("Password")
                    font.pixelSize: 20
                    color: "white"
                    wrapMode: Text.WordWrap
                }
                TextField {
                    id: passField
                    width: parent.width
                    text: utils.readSettings("lfmPass", "")
                    platformStyle: myTextFieldStyle
                    echoMode: TextInput.Password
                }
            }

            MyButton {
                text: qsTr("Login")
                x: 10
                enabled: !loginIn

                onClicked: {
                    loginIn = true
                    lfm.loginIn(userField.text, passField.text)
                }
            }

            Rectangle {
                x: 10
                width: parent.width -30
                height: 1
                color: currentTheme==="blanco"? "#4d4d4d" : themeColor
                opacity: 0.5
            }

            SelectionItem {
                width: parent.width -10
                title: qsTr("Language")
                /*model: ListModel {
                    ListElement { name: QT_TR_NOOP("English"); value: "en" }
                    ListElement { name: QT_TR_NOOP("Deutsch"); value: "de" }
                    ListElement { name: QT_TR_NOOP("Español"); value: "es" }
                    ListElement { name: QT_TR_NOOP("Français"); value: "fr" }
                    ListElement { name: QT_TR_NOOP("Italiano"); value: "it" }
                    ListElement { name: QT_TR_NOOP("日本語"); value: "jp" }
                    ListElement { name: QT_TR_NOOP("Polski"); value: "pl" }
                    ListElement { name: QT_TR_NOOP("Português"); value: "pt" }
                    ListElement { name: QT_TR_NOOP("Руccкий"); value: "ru" }
                    ListElement { name: QT_TR_NOOP("Svenska"); value: "se" }
                    ListElement { name: QT_TR_NOOP("Türkçe"); value: "tr" }
                    ListElement { name: QT_TR_NOOP("简体中文"); value: "zh" }
                }*/
                initialValue: utils.lang
                onValueChosen: {
                    utils.setLang(value)
                }
            }


            Row {
                x: 10
                spacing: 10
                width: parent.width -20
                CheckBox {
                    id: chkScrobble
                    checked: utils.scrobble=="true" ? true : false
                    platformStyle: myCheckBoxStyle
                    onCheckedChanged: { utils.setScrobble(checked ? "true" : "false") }
                }
                Text {
                    text: qsTr("Scrobble all songs")
                    anchors.verticalCenter: parent.verticalCenter
                    elide: Text.ElideRight
                    font.pixelSize: 22
                    color: "white"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { chkScrobble.checked = !chkScrobble.checked }
                    }
                }

            }
        }
    }

    ScrollDecorator {
        flickableItem: rectSong.visible ? rectSong : rectArtist.visible ? rectArtist :
                       rectAlbum.visible ? rectAlbum : lastfmSetup

    }


    Connections {
        target: lfm

        onLoginChanged: {
            if ( lfm.lfmKey() === "error" )
            {
                logresult.text = "Error login in. Try again."
                scroble.visible = false
                love.visible = false
            }
            else
            {
                logresult.text = "Logged as " + lfm.lfmUser()
                utils.setSettings("lfmUser", userField.text)
                utils.setSettings("lfmPass", passField.text)
                scroble.visible = true
                love.visible = true
            }
            loginIn = false
        }

        onLoveChanged: {
            if ( lfm.lfmLove() !== "loved" )
                love.iconSource = currentTheme==="sailfish"? "qrc:/images/sailfish-love.png" :
                                                             "image://theme/icon-m-toolbar-frequent-used-white"
            else
                love.iconSource = currentTheme==="sailfish"? "qrc:/images/sailfish-unlove.png" :"qrc:/images/unlove.png"
        }

        onScrobbleChanged: {
            if ( lfm.lfmScrobble() != "scrobbled" )
                scroble.enabled = true
            else {
                scrobbled = true
                scroble.enabled = false
            }
        }

        onBioChanged: {
            bioText.text = ""
            bioText.text = lfm.bioInfo()
            thumb.source = lfm.bioImg()
            artistReaded = true
        }
        onAlbumInfoChanged: {
            defaultAlbumArt = getCover(utils.thumbnail(currentSongArtist, currentSongAlbum))
            bioText2.text = ""
            bioText2.text = lfm.bioInfoAlbum()
            thumb2.source = defaultAlbumArt=="qrc:/images/nocover.jpg" ? getCover(lfm.bioImgAlbum())
                                                                       : getCover(defaultAlbumArt)
            albumReaded = true
        }
        onSongInfoChanged: {
            defaultAlbumArt = getCover(utils.thumbnail(currentSongArtist, currentSongAlbum))
            bioText3.text = ""
            bioText3.text = lfm.bioInfoSong()
            thumb3.source = defaultAlbumArt=="qrc:/images/nocover.jpg" ? getCover(lfm.bioImgSong())
                                                                       : getCover(defaultAlbumArt)
            songReaded = true
        }
    }


}
