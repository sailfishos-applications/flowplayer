import QtQuick 2.0
import Sailfish.Silica 1.0
import FlowPlayer 1.0

Page {
    id: root

    allowedOrientations: appWindow.pagesOrientations

    property string colored: "<style type='text/css'>a:link {color:" + Theme.highlightColor + "; text-decoration:none} " +
                             "a:visited {color:" + Theme.secondaryHighlightColor + "; text-decoration:none}</style>"

    SilicaFlickable {
        id: flickArea
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.height + Theme.paddingLarge
        clip: true

        Column {
            id: column
            width: parent.width -Theme.paddingLarge*2
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingLarge
            spacing: Theme.paddingLarge

            Header {
                title: qsTr("About")
                width: parent.width +Theme.paddingLarge
            }

            Row {
                width: parent.width
                spacing: Theme.paddingLarge

                Image {
                    source: "file:///usr/share/icons/hicolor/86x86/apps/flowplayer.png"
                }

                Column {
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter

                    Label {
                        id: main
                        color: Theme.highlightColor
                        text: "FlowPlayer"
                    }

                    Label {
                        color: Theme.secondaryHighlightColor
                        text: "version " + appVersion
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }

            }

            Label {
                color: Theme.secondaryColor
                text: qsTr("Original author:")
                font.pixelSize: Theme.fontSizeSmall
            }

            Label {
                color: Theme.secondaryColor
                text: "(C) 2015-2021 Matias Perez (CepiPerez)"
                font.pixelSize: Theme.fontSizeSmall
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            Label {
                color: Theme.secondaryColor
                text: qsTr("Contributors:")
                font.pixelSize: Theme.fontSizeSmall
            }

            Label {
                color: Theme.secondaryColor
                text: "(C) 2022-2024 olf (Olf0)" + "<br />" +
                      "(C) 2023-2024 Damien Caliste (dcaliste)"
                font.pixelSize: Theme.fontSizeSmall
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            Label {
                text: colored + qsTr("If you want to create a new translation or improve an extant one:") + "<br>" +
                      "<a href='https://app.transifex.com/olf/flowplayer/'>" + "Transifex - FlowPlayer</a>"
                onLinkActivated: Qt.openUrlExternally(link)
                font.pixelSize: Theme.fontSizeSmall
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Separator {
                width: parent.width
                color: Theme.secondaryColor
            }

            Label {
                text: colored + qsTr("You can support the original author of FlowPlayer by donating:")
                font.pixelSize: Theme.fontSizeSmall
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Image {
                source: "file://../paypal.png"
                height: Theme.itemSizeMedium
                fillMode: Image.PreserveAspectFit
                smooth: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.openUrlExternally("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=WTLJLQP2CSM7S")
                }
            }

        }

    }

}
