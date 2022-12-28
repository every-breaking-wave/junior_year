
import {message} from 'antd';
import axios from "axios";
import cookie from "react-cookies";
import {closeWebSocket, createWebSocket} from "../Websocket";
import PubSub from "pubsub-js";

export const loginUser = () => {
    return cookie.load('userId')
}

// 用户登录，保存cookie
export const saveIdCookie = (id) => {
    cookie.save('userId', id, {path: '/'})
}

export const logout = async () => {

    axios.post(`/api/user/logout`).then(
        response => {
            console.log("请求成功", response.data);
            if (response.data != null) {
                cookie.remove('userId')
                window.location.href = '/login'
                closeWebSocket()
                return response
            }
        },
    ).then((response) => {
            message.info("您的登陆时长为" + response.data + "s")
        }
    )
}

export const checkSession = (callback) => {
    axios.post(`/api/user/check`).then(
        response => {
            console.log("请求成功", response.data);
            if (response.data != null) {
                // Pubsub.publish('searchBook',{isLoading:false, bookList: response.data.bookList})
                message.info("ok")
                console.log(response.data)
                callback(response.data)
            }
        },
        error => {
            error.isPrototypeOf(error.message)
        }
    )
};


export async function getRole() {
    let role
    await axios.post(`/api/user/role`).then(
        response => {
            console.log("请求成功", response.data);
            if (response.data != null) {
                // Pubsub.publish('searchBook',{isLoading:false, bookList: response.data.bookList})
                console.log(response.data)
                role = response.data
                // callback(role)
            }
        },
        error => {
            error.isPrototypeOf(error.message)
        }
    )
    return role
}


export const banUser = async (userId) => {
    const res = await fetch("/api/admin/ban-user", {
        method: "POST",
        headers: new Headers({
            "Content-Type": "application/json"
        }),
        body: JSON.stringify(userId)
    }).then(
        () => {

        }
    )
    // return await res.json()
}


export function login(userAccount, userPassword, history) {
    let flag = false
    axios.post(`/api/user/login`, {
        userAccount: userAccount,
        userPassword: userPassword
    }).then(
        response => {
            if (response.data.status == 'USER_ALL_OK') {
                message.info("登陆成功，祝您购物愉快")
                let userId = cookie.load('userId')
                let url="ws://localhost:8080/websocket/create-order/"+response.data.id;//服务端连接的url
                createWebSocket(url)
                console.log("websocket connection created")
                // let messageSocket=null;
                // messageSocket = PubSub.subscribe('message','getMsg')
                saveIdCookie(response.data.id)
                history.push({pathname: '/default'})
            } else if ("USER_NOT_EXIST" == response.data.status) {
                message.error("登录失败：用户不存在或密码错误")
            } else if ("USER_BEEN_BANNED" == response.data.status) {
                message.error("登录失败：用户被禁用")
            } else if ("USER_ACCOUNT_PASSWORD_NULL" == response.data.status) {
                message.error("登录失败：账号或密码不能为空")
            } else if ("USER_ACCOUNT_ILLEGAL" == response.data.status) {
                message.error("登录失败：账号不能包含非法字符")
            }
        }
    )
}


export const register = (userAccount, userPassword, repeatPassword, email) => {
    axios.post(`/api/user/register`, {
        userAccount: userAccount,
        userPassword: userPassword,
        repeatPassword: repeatPassword,
        email: email
    }).then(
        response => {
            console.log("请求成功", response.data);
            if (response.data.status == 'USER_ALL_OK') {
                message.info("注册成功，祝您购物愉快")
                // let { history } = this.props
                // history.push({ pathname: '/login' })
                window.location.href = '/login'
            } else if ("USER_ALREADY_EXIST" == response.data.status) {
                message.error("注册失败：用户已存在")
            } else if ("USER_EMAIL_ILLEGAL" == response.data.status) {
                message.error("注册失败：邮箱格式错误")
            } else if ("PASSWORD_NOT_MATE_REPEAT_PASSWORD" == response.data.status) {
                message.error("注册失败：两次输入的密码不匹配")
            } else if ("USER_ACCOUNT_ILLEGAL" == response.data.status) {
                message.error("注册失败：账号不能包含非法字符")
            } else if ("USER_ACCOUNT_PASSWORD_NULL" == response.data.status) {
                message.error("注册失败：账号、密码和邮箱不能为空")
            }
        },
        error => {
            console.log("请求失败", error);
        }
    )
}

