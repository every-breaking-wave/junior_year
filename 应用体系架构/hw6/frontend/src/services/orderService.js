import axios from "axios";
import Pubsub from "pubsub-js";
import {message} from "antd";
import cookie from "react-cookies";


export const orderService = (userId, callback) => {

    axios.post(`/api/order/create`, {
        userId: userId,
    }).then(
        response => {
            console.log("请求成功", response.data);
            message.info("订单创建添加成功")
            console.log(response.data.id)
            // history.push("/userCenter")
            callback(response.data.id)
        },
        error => { console.log("请求失败", error); }
    )

};


export const createOrderItem = (bookList,orderId) => {

    axios.post(`/api/orderitem/create`, {
       bookList:bookList,
       orderId:orderId
    }).then(
        response => {
            console.log("请求成功", response.data);
        },
        error => { console.log("请求失败", error); }
    )
};

export  const getOrderList = (callback) =>{
    axios.post(`/api/orderitem/list`, {
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            callback(response.data)
        },
        error => { console.log("请求失败", error); }
    )
}

export const getOrderItemsById = async (userId) => {
    axios.post(`/api/orderitem/list`, {
        userId: userId
    }).then(
        response => {
            console.log("请求成功", response.data);
        },
        error => { console.log("请求失败", error); }
    )
}

export const cancelOrder = async (userId) => {
    axios.post(`/api/admin/delete`, {
        userId: userId
    }).then(
        response => {
            console.log("请求成功", response.data);
        },
        error => { console.log("请求失败", error); }
    )
}

export const getOrderById = async (userId) => {
    axios.post(`/api/order/get/${userId}`, {
        userId: userId
    }).then(
        response => {
            console.log("请求成功", response.data);
        },
        error => { console.log("请求失败", error); }
    )
}

export const compare = (property) => {
    return function(a,b){
        var value1 = a[property];
        var value2 = b[property];
        return value2 - value1;
    }
}