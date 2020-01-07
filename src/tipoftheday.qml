import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Rectangle {
    id: root

    property font font: Qt.font({ family: "Avenir Next" })

    width: 410
    height: 390
    focus: true

    /*gradient: Gradient {
        GradientStop { position: 0.0; color: "#5A503C" }
        GradientStop { position: 1.0; color: "#503737" }
    }*/

    function playNextTipOfTheDay() {
        if (_m.currentIndex === _m.model.count - 1) {
            _m.currentIndex = 0
            return
        }

        _m.currentIndex += 1
    }

    function playPreviousTipOfTheDay() {
        if (_m.currentIndex === 0) {
            _m.currentIndex = _m.model.count - 1
            return
        }

        _m.currentIndex -= 1
    }

    function updateDontShowAgain(dontShowAgain) {
        // MainWindow.dontShowTipOfTheDay(dontShowAgain)
        console.log("Don't show this again", dontShowAgain)
    }

    Image {
        anchors.fill: parent
        source: "qrc:/images/background.png"
        fillMode: Image.Stretch
    }

    Rectangle {
        anchors.fill: parent
        opacity: 0.80
        color: "#2B2B29"
    }

    ColumnLayout {
        anchors {
            fill: parent
            margins: 16
        }
        spacing: 8

        RowLayout {
            spacing: 32

            Layout.fillWidth: true

            Image {
                source: "qrc:/images/lightbulb.png"
            }

            Label {
                font {
                    family: root.font.family
                    pixelSize: 31
                }
                color: "white"
                text: qsTr("Did you know...?")

                Layout.fillWidth: true
            }
        }

        Rectangle {
            height: 2
            color: "black"

            Layout.fillWidth: true
        }

        Item {
            clip: true

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.2
            }

            Text {
                id: tipOfTheDay
                anchors {
                    fill: parent
                    margins: 8
                }
                font {
                    family: root.font.family
                    pixelSize: 16
                }
                color: "white"
                text: _m.model.get(_m.currentIndex).data
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            spacing: 4

            Layout.fillWidth: true

            CheckBox {
                id: checkBox
                indicator: Rectangle {
                    implicitWidth: 11
                    implicitHeight: 11
                    x: checkBox.leftPadding
                    y: parent.height / 2 - height / 2
                    border {
                        width: 1
                        color: "white"
                    }
                    color: "transparent"

                    Rectangle {
                        anchors.centerIn: parent
                        width: 8
                        height: 8
                        color: checkBox.checked ? "white" : "transparent"
                        visible: checkBox.checked
                    }
                }
                contentItem: Label {
                    font {
                        family: root.font.family
                        pixelSize: 14
                        weight: Font.Light
                    }
                    leftPadding: checkBox.indicator.width + checkBox.spacing
                    verticalAlignment: Text.AlignVCenter
                    opacity: checkBox.enabled ? 1.0 : 0.3
                    color: "white"
                    text: checkBox.text
                }
                text: qsTr("Don't show this again")

                onCheckedChanged: root.updateDontShowAgain(checkBox.checked)
            }

            Item {
                height: 1

                Layout.fillWidth: true
            }

            Button {
                implicitWidth: contentItem.implicitWidth + padding * 2
                padding: 4
                font {
                    family: root.font.family
                    pixelSize: 14
                }
                flat: true
                text: "\u2190 <u>P</u>revious"
                contentItem: Text {
                    anchors.centerIn: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font: parent.font
                    color: "#FAE232"
                    text: parent.text
                }

                Layout.alignment: Qt.AlignVCenter

                onClicked: root.playPreviousTipOfTheDay()
            }

            Button {
                implicitWidth: contentItem.implicitWidth + padding * 2
                padding: 4
                font {
                    family: root.font.family
                    pixelSize: 14
                }
                flat: true
                text: "<u>N</u>ext \u2192"
                contentItem: Text {
                    anchors.centerIn: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font: parent.font
                    color: "#FAE232"
                    text: parent.text
                }

                Layout.alignment: Qt.AlignVCenter

                onClicked: root.playNextTipOfTheDay()
            }
        }
    }

    Keys.onPressed: {
        switch (event.key) {
        case 78: // N
            root.playNextTipOfTheDay()
            break
        case 80: // P
            root.playPreviousTipOfTheDay()
            break
        default:
            // do nothing
            break
        }
    }

    QtObject { // private section
            id: _m

            property int currentIndex: 0

            readonly property ListModel model: ListModel {
                ListElement {
                    data: "First tip of the day..."
                }

                ListElement {
                    data: "Multyline<br>tip<br>of<br>the<br>day."
                }

                ListElement {
                    data: "Third tip of the day."
                }
            }
    }
}
