import QtQuick 1.0

Rectangle {
    anchors.fill: parent

    Rectangle {
        x: 20
        width: parent.width -40
        height: 50
        color: "transparent"

        Image {
            id: heading
            anchors.top: parent.top
            anchors.topMargin: 10
            x: -20
            source: "qrc:/images/lastfm2.png"
            smooth: true
            height: 28
            width: 120
            fillMode: Image.PreserveAspectFit
        }

        Image {
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.right: parent.right
            //anchors.rightMargin: 18
            height: 32; width: 32; smooth: true
            source: "image://theme/icon-m-common-refresh"
            opacity: 0.6
            visible: rectArtist.visible && bioText.text!="Fetching artist information" ? true :
                     rectAlbum.visible && bioText2.text!="Fetching album information" ? true :
                     rectSong.visible && bioText3.text!="Fetching track information" ? true : false
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if ( rectArtist.visible == true )
                    {
                        bioText.text= "Fetching artist information"
                        thumb.source = ""
                        lfm.getBio(currentSongArtist)
                    }
                    else if ( rectAlbum.visible == true )
                    {
                        bioText2.text= "Fetching album information"
                        //thumb2.source = ""
                        lfm.getAlbumBio(currentSongArtist, currentSongAlbum)
                    }
                    else
                    {
                        bioText3.text= "Fetching track information"
                        //thumb3.source = ""
                        lfm.getSongBio(currentSongArtist, currentSongTitle)
                    }
                }
            }
        }
    }

    Rectangle {
        x: 20
        id: line
        width: parent.width - 40
        height: 1
        color: currentTheme==="blanco"? "#4d4d4d" : themeColor
        opacity: 0.5
    }

}

