import React, {Component} from 'react';
import cookie from "react-cookies";
import {useCookies} from 'react-cookie';
import axios from "axios";
import {message, Typography, Tag, Button, Space, Collapse, Descriptions} from "antd";
import {history} from "../utils/history";
import HeaderL from "../components/Header/Header";
import Footer from "../components/Footer";
import {banUser} from "../services/userService";

export default class UserManageView extends Component {
    constructor(props) {
        super(props);
        this.state = {sum: 0}
        this.state = {userList: [], userState: []}
    }

    // TODO: 页面不刷新
    handleBan(event, user, index){
        console.log(this.state.userState)
        let state = this.state.userState
        banUser(user.id)
        this.setState({userState :user.id})
        let newState = this.state.userState
        console.log(index)
        newState[index] = !newState[index];
        this.setState({userState: newState})
        console.log(this.state.userState)
    }


    componentWillMount() {
        console.log(this.state.list)
        axios.post(`/api/admin/get-all-user`).then(
            response => {
                console.log("请求成功", response.data);
                if (response.data != null) {
                    let newState = this.state.userState
                    // history.push({pathname: '/default'})
                    this.setState({userList: response.data})
                    console.log(this.state.userList)
                    this.state.userList.map((item, index)=>{
                        newState[index] =  item.userStatus
                        this.setState({userState : newState})
                    })
                    console.log(newState)
                }
            },
            error => {
                message.error("你没有此权限！")
                history.push("/")
            }
        )
    }

    render() {
        // this.componentDidMount()
        const userList = this.state.userList || []
        const states = this.state.userState || []
        console.log(states)
        // const bookList = cookie.loadAll().list;
        // console.log(this.props)
        console.log(userList)
        return (
            <div>
                <HeaderL/>
                    <Collapse accordion={true} style={{marginBottom: 200}}>
                        <Typography.Text strong>
                            {/*创建时间：{item.createTime}*/}
                        </Typography.Text>
                        {userList.map((item, index) => {
                            return (
                                <Collapse.Panel
                                    key={index}
                                    onClick={() => this.handlePanelClick(item.userId, index)}
                                    header={
                                        <div style={{width: "100%"}}>
                                            <Typography.Text strong>
                                                用户账号：{item.userAccount}
                                            </Typography.Text>
                                            <div style={{float: "right"}}>
                                                {
                                                    states[index] == true?
                                                        <Button
                                                        size="small"
                                                        type="primary"
                                                        color= "red"
                                                        onClick={(event) => this.handleBan(event,item, index)}
                                                         >
                                                         解除禁用
                                                         </Button> :
                                                        <Button
                                                            size="small"
                                                            type="primary"
                                                            onClick={(event) => this.handleBan(event, item, index)}
                                                        >
                                                            禁用
                                                        </Button>

                                                }

                                            </div>

                                        </div>
                                    }

                                >
                                    <Descriptions>
                                        <Descriptions.Item label="用户ID">
                                            {item.id}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="创建时间">
                                            {item.createTime}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="用户状态">
                                            {states[index] ? "已禁用" : "正常"}
                                        </Descriptions.Item>

                                    </Descriptions>
                                </Collapse.Panel>
                            )
                        })}
                    </Collapse>
                <Footer/>
            </div>
        );
    }
}

