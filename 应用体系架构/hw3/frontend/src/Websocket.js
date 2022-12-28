import  {PubSub}  from 'pubsub-js';
let websocket,
    lockReconnect = false;
let createWebSocket = (url) => {
    websocket = new WebSocket(url);
    console.log('ws connect')
    websocket.onopen = function() {
        console.log('ws open')
        heartCheck.reset().start();
    };
    websocket.onerror = function() {
        reconnect(url);
    };
    websocket.onclose = function(e) {
        heartCheck.reset()
        console.log('ws closed: ' + e.code + ' ' + e.reason + ' ' + e.wasClean);
    };
    websocket.onmessage = function(event) {
        lockReconnect = true;
        //event 为服务端传输的消息，在这里可以处理
        let data=event.data;//把获取到的消息处理成字典，方便后期使用
        PubSub.publish('message',data); //发布接收到的消息 'message' 为发布消息的名称，data 为发布的消息
        let message =  JSON.parse(data)
        switch (message.status) {
            case 'ORDER_ALL_OK':
                console.log("reach here")
                alert("订单创建成功! 总计花费："+ message.price.toFixed(2) + '元')
                break
            default:
                alert("订单创建失败")
        }
    };
};
let reconnect = (url) => {
    if (lockReconnect) return;
    // 没连接上会一直重连，设置延迟避免请求过多
    setTimeout(function() {
        createWebSocket(url);
        lockReconnect = false;
    }, 4000);
};
let heartCheck = {
    timeout: 6000, // 6秒
    timeoutObj: null,
    reset: function() {
        clearInterval(this.timeoutObj);
        return this;
    },
    start: function() {
        this.timeoutObj = setInterval(function() {
            // 这里发送一个心跳，后端收到后，返回一个心跳消息，
            // onmessage拿到返回的心跳就说明连接正常
            websocket.send('HeartBeat');
            // self.serverTimeoutObj = setTimeout(function(){//如果超过一定时间还没重置，说明后端主动断开了
            //     websocket.close();     //如果onclose会执行reconnect，我们执行ws.close()就行了.如果直接执行reconnect 会触发onclose导致重连两次
            // }, self.timeout)
        }, this.timeout);
    },
};
// 关闭连接
let closeWebSocket = () => {
    console.log("reach here")
    // websocket && websocket.close();
};
export { websocket, createWebSocket, closeWebSocket };
