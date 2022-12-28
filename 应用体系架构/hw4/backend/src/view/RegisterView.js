import React from 'react';
import { Input, Form, Button, message } from 'antd';
// import WrappedLoginForm from '../components/LoginForm';
import { withRouter, Link, useNavigate } from "react-router-dom";
import '../css/login.css'
import '../css/base.css'
import {register} from "../services/userService";


class LoginView extends React.Component {

    constructor(props) {
        super(props);
        this.state = { userAccount: "", userPassword: "" ,repeatPassword: "", email: ""}
    }

    handleInputChange = (event, name) => {
        const value = event.target.value
        switch (name) {
            case "userAccount":
                this.setState({ userAccount: value })
                break
            case "userPassword":
                this.setState({ userPassword: value })
                break
            case "repeatPassword":
                this.setState({ repeatPassword: value })
                break
            case "email":
                this.setState({ email: value })
                break
        }
    }


    handleRegister = () => {
        const userAccount = this.state.userAccount
        const userPassword = this.state.userPassword
        const repeatPassword = this.state.repeatPassword
        const email = this.state.email
        register(userAccount, userPassword, repeatPassword, email)
    }



    render() {
        return (
            <div className="BodyBg t l">
                <div className="BgPicture t"></div>
                <div className="bodyMiddle c" style={{ height:30 }}>
                    <div className="bodyMain" style={{ height:380 }}>
                        <div className="MainContent">
                            <em>登录EBook</em>
                            <div className="input1 l" style={{ marginLeft: 30 }}>
                                <div className="userIcon leftIcon"></div>
                                <div className="user u1 l">
                                    <div className="clear cl1"></div>
                                    <Form
                                        label="用户账号"
                                        name="userAccount"
                                        rules={[
                                            {
                                                required: true,
                                                message: "请输入新账号",
                                            }
                                        ]}
                                    >
                                        <input
                                            ref={a => this.keyWordElement = a}
                                            type="text"
                                            placeholder="请输入新账号"
                                            style={{ width: "230px" }}
                                            onChange={(e) => this.handleInputChange(e, "userAccount")}
                                        />
                                    </Form>
                                </div>
                            </div>
                            <div className="input2 l" style={{ marginLeft: 30 }}>
                                <div className="pswordIcon leftIcon"></div>
                                <div className="user l">
                                    <div className="clear cl2"></div>
                                    <Form
                                        label="用户密码"
                                        name="userPassword"
                                        rules={[
                                            {
                                                required: true,
                                                message: "请输入新密码",
                                            }
                                        ]}
                                    >
                                        <input
                                            ref={c => this.keyWordElement = c}
                                            type="text"
                                            placeholder="请输入新密码"
                                            style={{ width: "230px" }}
                                            onChange={(e) => this.handleInputChange(e, "userPassword")}
                                        />
                                    </Form>
                                </div>
                            </div>
                            <div className="input2 l" style={{ marginLeft: 30 }}>
                                <div className="pswordIcon leftIcon"></div>
                                <div className="user l">
                                    <div className="clear cl2"></div>
                                    <Form
                                        label="重复密码"
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
                                            placeholder="请确认密码"
                                            style={{ width: "230px" }}
                                            onChange={(e) => this.handleInputChange(e, "repeatPassword")}
                                        />
                                    </Form>
                                </div>
                            </div>
                            <div className="input2 l" style={{ marginLeft: 30 }}>
                                <div className="pswordIcon leftIcon"></div>
                                <div className="user l">
                                    <div className="clear cl2"></div>
                                    <Form
                                        label="重复密码"
                                        name="userPassword"
                                        rules={[
                                            {
                                                required: true,
                                                message: "请输入有效邮箱",
                                            }
                                        ]}
                                    >
                                        <input
                                            ref={c => this.keyWordElement = c}
                                            type="text"
                                            placeholder="请输入邮箱"
                                            style={{ width: "230px" }}
                                            onChange={(e) => this.handleInputChange(e, "email")}
                                        />
                                    </Form>
                                </div>
                            </div>

                            <Button
                                type="primary"
                                htmlType="submit"
                                style={{ width: "80%", marginLeft: "10%" ,marginTop:40}}
                                onClick={this.handleRegister}
                            >
                                立刻注册
                            </Button>


                            <div className="forgetpass l">
                                <p  style={{marginLeft:170}}>
                                    已有账号？
                                    <Link to="/login" className="red">马上登录</Link>
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


