import React from 'react';
import {Form, Button, message} from 'antd';
import WrappedLoginForm from '../components/LoginForm';
import {Link} from "react-router-dom";
import '../css/login.css'
import '../css/base.css'
import axios from 'axios';
import '../services/userService'
import {history} from "../utils/history";


import {login, saveIdCookie} from "../services/userService";


class LoginView extends React.Component {


    constructor(props) {
        super(props);
        this.state = {userAccount: "", userPassword: ""}
        this.handleLogin = this.handleLogin.bind(this)
    }

    handleInputChange = (event, name) => {
        const value = event.target.value
        switch (name) {
            case "userAccount":
                this.setState({userAccount: value})
                break
            case "userPassword":
                this.setState({userPassword: value})
                break
        }
    }


    async handleLogin() {

        const userAccount = this.state.userAccount
        const userPassword = this.state.userPassword
        console.log(userAccount)
        console.log(userPassword)
        let {history} = this.props
        login(userAccount, userPassword, history)
    }


    render() {
        return (
            <div className="BodyBg t l">
                <div className="BgPicture t"></div>
                <div className="bodyMiddle c" style={{height: 50}}>
                    <div className="bodyMain" style={{height: 300}}>
                        <div className="MainContent">
                            <em>登录EBook</em>
                            <div className="input1 l" style={{marginLeft: 30}}>
                                <div className="userIcon leftIcon"></div>
                                <div className="user u1 l">
                                    <div className="clear cl1"></div>
                                    <Form
                                        label="用户账号"
                                        name="userAccount"
                                        rules={[
                                            {
                                                required: true,
                                                message: "请输入有效的账号",
                                            }
                                        ]}
                                    >
                                        <input
                                            ref={a => this.keyWordElement = a}
                                            type="text"
                                            placeholder="请输入账号"
                                            style={{width: "230px"}}
                                            onChange={(e) => this.handleInputChange(e, "userAccount")}
                                        />
                                    </Form>
                                </div>
                            </div>
                            <div className="input2 l" style={{marginLeft: 30}}>
                                <div className="pswordIcon leftIcon"></div>
                                <div className="user l">
                                    <div className="clear cl2"></div>
                                    <Form
                                        label="用户密码"
                                        name="userPassword"
                                        rules={[
                                            {
                                                required: true,
                                                message: "请输入有效密码",
                                            }
                                        ]}
                                    >
                                        <input
                                            ref={c => this.keyWordElement = c}
                                            type="text"
                                            placeholder="请输入密码"
                                            style={{width: "230px"}}
                                            onChange={(e) => this.handleInputChange(e, "userPassword")}
                                        />
                                    </Form>
                                </div>
                            </div>

                            <Button
                                type="primary"
                                htmlType="submit"
                                style={{width: "80%", marginLeft: "10%", marginTop: 40}}
                                onClick={this.handleLogin}
                            >
                                登录
                            </Button>


                            <div className="forgetpass l">
                                <p>
                                    <a href="/#">忘记密码？</a>
                                    还没有账号？
                                    <Link to="/register" className="red">免费注册</Link>
                                </p>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

        );

    }
}

export default LoginView;


