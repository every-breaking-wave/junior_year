import React from "react";
import '../../css/header.css'
import '../../css/base.css'
import LeftNav from "../leftNav";
import {getRole, logout} from "../../services/userService";
import axios from "axios";
import {message, Tooltip} from "antd";
import Search from "./Search";
import TopBar from "./TopBar";


class HeaderL extends React.Component {

    constructor(props) {
        super(props);
        this.state = {list: [], role: false};
        this.search = this.search.bind(this);
    }

    search = (e) => {
        var arr = this.state.list.filter((item, index) => item.name.indexOf(e.target.value) > -1);
        this.setState({list: arr});
    };

    async componentDidMount() {
        const userRole = await getRole();
        this.setState({role: userRole})
        console.log(this.state.role)
        console.log({userRole})
        if (typeof this.state.role == "undefined") {
            this.setState({role: false})
        }
        console.log(this.state.role)
    }


    handleOnclick() {
        axios.post(`/api/car/cartList`, {}).then(
            response => {
                console.log("请求成功", response.data);
                message.info("购物车添加成功")
            },
            error => {
                console.log("请求失败", error);
            }
        )
    }

    render() {
        return (

            <div>
                <TopBar/>
                <Search/>
            </div>
        )
    }
}

export default HeaderL




