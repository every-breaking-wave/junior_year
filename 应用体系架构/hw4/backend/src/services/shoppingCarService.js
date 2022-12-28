import axios from "axios";
import {message} from "antd";
import cookie from "react-cookies";


export const addCart = (bookId)=>{
    axios.post(`/api/cart/addCart`, {
        bookId: bookId,
        count: 1,
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            message.info("购物车添加成功")
        },
        error => { console.log("请求失败", error); }
    )
}


export const minCart = (bookId)=>{
    axios.post(`/api/cart/minCart`, {
        bookId: bookId,
        count: 1,
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            message.info("购物车减少成功")
        },
        error => { console.log("请求失败", error); }
    )
}
export const changeCart = (num, bookId)=>{
    axios.post(`/api/cart/changeCart`, {
        bookId: bookId,
        count: num,
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            // message.info("购物车改变成功")
        },
        error => { console.log("请求失败", error); }
    )
}

export  const delCart = (bookId)=>{
    axios.post(`/api/cart/delCart`, {
        bookId: bookId,
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            message.info("购物车删除成功")
        },
        error => { console.log("请求失败", error); }
    )
}

 export function getCar(callback){
    axios.post(`/api/cart/cartList`, {
        userId: cookie.load("userId")
    }).then(
        response => {
            console.log("请求成功", response.data);
            callback(response.data.bookInCarList)
        },
        error => { console.log("请求失败", error); }
    )
}
