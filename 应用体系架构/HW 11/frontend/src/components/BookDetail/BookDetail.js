import React from 'react';
import {Descriptions, Button, Card, InputNumber, message} from 'antd';
import '../../css/bookDetail.css'
import Input from "./Input"
import {Link} from 'react-router-dom'
import cookie from 'react-cookies'
import {getBook} from "../../services/bookService";
import Pubsub from "pubsub-js";
import axios from "axios";
import {saveIdCookie} from "../../services/userService";


export default class BookDetail extends React.Component {

    constructor(props) {
        super(props);
        this.state = {book: null, num: 1, carId: 0};
    }

    onChange(value) {
        console.log('changed', value);
        this.setState({num: value})
    }

    handleClick() {
        const userId = cookie.load("userId")
        console.log(userId)
        // console.log(this.props.info.id)
        axios.post(`/api/cart/addCart`, {
            bookId: this.props.info.id,
            count: this.state.num,
            userId: userId
        }).then(
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
        const {info} = this.props;
        if (info == null) {
            return null;
        }

        return (
            <div id="productDetail t" style={{background: "white"}}>
                <div className="detailContent c">
                    <div className="productImg">
                        <img className="picture l" src={info.cover} style={{width: 300}}/>
                        <div className="bigImgVideo">
                        </div>
                        <div className="square"></div>

                    </div>
                    <div className="productInfo l">
                        <div className="productTitle l">
                            <div className="titleBox l">
                                <h1>{info.bookName}</h1>
                                <p className="p2">{info.bookDescription}</p>
                            </div>
                        </div>
                        <div className="productPrice l">
                            <p className="one l">价格</p>
                            <p className="three l">￥ {info.price}</p>
                            <p className="four l">新品</p>
                        </div>

                        <div className="size l">
                            <p className="actTitle l">库存</p>
                            <p className="size1 l"><a href="#"> {info.inventory }</a></p>
                        </div>
                        <div>


                            <div className="goButton l">
                                {/*<div className="number l">*/}
                                <InputNumber min={1} max={10} defaultValue={1} onChange={this.onChange.bind(this)}
                                             size={"large"}/>

                            </div>

                            <div className="addCar l" style={{marginTop: 20, marginLeft: 20}}>

                                <a
                                      onClick={this.handleClick.bind(this)}
                                >
                                    <p>
                                        加入购物车
                                    </p>
                                </a>
                                <div className="goCar l"></div>
                            </div>

                        </div>
                    </div>
                </div>
            </div>


        )

    }

}
