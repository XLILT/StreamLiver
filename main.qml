import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Column {
    // id: root_rec
    // color: "green"
    // width: 200
    height: 150

    // 发送给 Qt Widgets 的信号
    signal selectCamera(string camera)
    signal beginLive
    signal setStreamUrl(string url)
    // 从 Qt Widgets 接收到的信号
    signal cameraListChanged

    spacing: 10

    Row {
        // id: camera_area
        spacing: 2

        Text {
            id: camera_des
            text: qsTr("选择摄像头:")
            font.pointSize: 16
            // anchors.centerIn: parent
        }

        ComboBox {
            id: camera_combox
            // anchors.left: camera_des.right
            // anchors.leftMargin: 10
            width: 700
            height: 25
            model: ListModel {
                id: camera_list
            }

            onActivated: selectCamera(currentText)
        }
    }

    Row {
        spacing: 2

        Text {
            // id:stream_des
            text: qsTr("输入流地址:")
            font.pointSize: 18
            // anchors.centerIn: parent
        }

        TextField {
            placeholderText: qsTr("流地址")
            width: 300
            height: 30
            selectByMouse: true

            onEditingFinished: setStreamUrl(displayText)
        }
    }

    Button {
        id: live_btn
        text: qsTr("开始直播")
        // onClicked: start_live()
        // anchors.top: camera_des.bottom
        // anchors.topMargin: 10

        onClicked: beginLive()
    }

//    MouseArea {
//        anchors.fill: parent
//        onClicked: qmlSignal()
//    }

    // 信号处理程序（处理从 Qt Widgets 接收到的信号）
//    onCSignal: {
//        root.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1);
//        myText.text = "Call qml signal handler"
//    }

    onCameraListChanged: {
        var cam_list = central_widget.cameraList

        for(var i = 0; i < cam_list.length; i++) {
            camera_list.append({text: cam_list[0]})
        }

//        if(camera_combox.currentIndex < 0 && camera_combox.count > 0) {
//            camera_combox.currentIndex = 0;
//        }
    }
}
