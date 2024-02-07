import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0
import FlowPlayer 1.0

FolderPickerPage {
    dialogTitle: qsTr("Select folder")
    showSystemFiles: false
    onSelectedPathChanged: {
        utils.addFolderToList(selectedPath)
    }
}
