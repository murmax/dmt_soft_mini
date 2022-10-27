import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MyNamespace 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Project")

    FileDataModel {
        id: theFileDataModel
    }
    HardwareDataModel {
        id: theHardwareDataModel
    }
    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            theFileDataModel.updateSelf()
            theHardwareDataModel.updateSelf()
        }
    }

    function onBtnSetFilterClick() {


        /*
        theFileDataModel.filterMp3 = "0"
        theFileDataModel.filterMp4 = "1"
        theFileDataModel.filterJpg = "0"
        theFileDataModel.filterDateRecordStart = "2022-10-13 08:00:00"
        theFileDataModel.filterDateRecordEnd = "2022-10-13 11:00:00"
        theFileDataModel.filterDateDownloadStart = "2022-10-13 08:29:00"
        theFileDataModel.filterDateDownloadEnd = "2022-10-13 08:30:00"
        theFileDataModel.filterSerialNumber = "124"
        */
        theFileDataModel.filterMp3 = ""
        theFileDataModel.filterMp4 = ""
        theFileDataModel.filterJpg = ""
        theFileDataModel.filterDateRecordStart = ""
        theFileDataModel.filterDateRecordEnd = ""
        theFileDataModel.filterDateDownloadStart = ""
        theFileDataModel.filterDateDownloadEnd = ""
        theFileDataModel.filterSerialNumber = ""
    }

    function onBtnPlusNumClick() {
        theFileDataModel.pageNum = theFileDataModel.pageNum + 1
        pageNumText.text = theFileDataModel.pageNum
    }
    function onBtnMinusNumClick() {
        theFileDataModel.pageNum = theFileDataModel.pageNum - 1
        pageNumText.text = theFileDataModel.pageNum
    }
    function onBtnPlusSizeClick() {
        theFileDataModel.pageSize = theFileDataModel.pageSize + 1
        pageSizeText.text = theFileDataModel.pageSize
    }
    function onBtnMinusSizeClick() {
        theFileDataModel.pageSize = theFileDataModel.pageSize - 1
        pageSizeText.text = theFileDataModel.pageSize
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 800
            Layout.preferredHeight: 30
            RowLayout {
                spacing: 5
                Button {
                    width: 90
                    height: 30
                    id: testButton1
                    text: qsTr("setFilter")
                    onClicked: onBtnSetFilterClick()
                }
                Button {
                    width: 30
                    height: 30
                    id: btnPlusNum
                    text: qsTr("Num+")
                    onClicked: onBtnPlusNumClick()
                }
                Rectangle {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    Text {
                        id: pageNumText
                        anchors.centerIn: parent
                        text: theFileDataModel.pageNum
                    }
                }
                Button {
                    width: 30
                    height: 30
                    id: btnMinusNum
                    text: qsTr("Num-")
                    onClicked: onBtnMinusNumClick()
                }
                Button {
                    width: 30
                    height: 30
                    id: btnPlusSize
                    text: qsTr("Size+")
                    onClicked: onBtnPlusSizeClick()
                }
                Rectangle {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    Text {
                        id: pageSizeText
                        anchors.centerIn: parent
                        text: theFileDataModel.pageSize
                    }
                }
                Button {
                    width: 30
                    height: 30
                    id: btnMinusSize
                    text: qsTr("Size-")
                    onClicked: onBtnMinusSizeClick()
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            HorizontalHeaderView {
                id: horizontalHeaderFiles
                reuseItems: false
                syncView: tableViewFiles
                height: 30
                boundsBehavior: Flickable.StopAtBounds
                delegate: Rectangle {
                    id: columnHeader
                    color: "#eee"
                    border.color: "#bbb"
                    border.width: 1
                    implicitHeight: 30
                    Text {
                        id: headerText
                        anchors.centerIn: parent
                        text: display
                    }
                }
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            TableView {
                id: tableViewFiles
                anchors.fill: parent
                clip: true
                model: theFileDataModel
                delegate: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    border.color: "#bbb"
                    border.width: 1
                    Text {
                        text: display
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            HorizontalHeaderView {
                id: horizontalHeaderDevices
                reuseItems: false
                syncView: tableViewDevices
                height: 30
                boundsBehavior: Flickable.StopAtBounds
                delegate: Rectangle {
                    id: columnHeaderDevices
                    color: "#eee"
                    border.color: "#bbb"
                    border.width: 1
                    implicitHeight: 30
                    Text {
                        id: headerTextDevices
                        anchors.centerIn: parent
                        text: display
                    }
                }
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            TableView {
                id: tableViewDevices
                anchors.fill: parent
                clip: true
                model: theHardwareDataModel
                delegate: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 50
                    border.color: "#bbb"
                    border.width: 1
                    Text {
                        text: display
                    }
                }
            }
        }
    }
}
