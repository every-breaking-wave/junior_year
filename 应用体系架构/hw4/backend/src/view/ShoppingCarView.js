import React from 'react';
import {Link, withRouter} from "react-router-dom";
import CarList from "../components/ShoppingCar/CarList";
import '../css/shoppingCar.css'
import '../css/base.css'
import cookie from "react-cookies";
import {orderService, createOrderItem} from "../services/orderService";
import "../services/orderService"
import axios from "axios";
import TopBar from "../components/Header/TopBar";
import Footer from "../components/Footer";

export default class ShoppingCarView extends React.Component {

    constructor(props) {
        super(props);
        this.state = { books:[], orderId:0 , sum : 0}
        // this.setState({ books: listBrand })
        this.componentDidMount = this.componentDidMount.bind(this);
        this.calculateSum = this.calculateSum.bind(this)
    }

    componentDidMount(){
        axios.post(`/api/cart/cartList`, {
            userId:cookie.load("userId")
        }).then(
            response => {
                console.log("请求成功", response.data);
                this.setState({books:response.data})
                this.calculateSum(response.data)
                console.log(this.state.sum)
            },
            error => { console.log("请求失败", error); }
        )
    }

    calculateSum(books){
        let sum = 0
        console.log(books.bookInCarList)
        console.log(books.length)
        for (let i = 0; i <  books.bookInCarList.length; i++) {
            sum += books.bookInCarList[i].book.price * books.bookInCarList[i].number
        }
        sum =  sum.toFixed(2)
        console.log(sum)
        this.setState({sum : sum})
    }


    callback=(orderId)=>{
       this.setState({orderId:orderId})
        console.log(this.state.orderId)
    }

    createOrder(){
        const userId = cookie.load("userId")
        console.log(userId)
        //创建 order 并获得order的id
        const orderId = 0
        orderService(userId,this.callback)
        this.componentDidMount()
    }
    searchBooks = (bookName)=>{
        console.log("bookView"+bookName);
    }

    render() {
        const bookList = this.state.books;
        console.log(this.props)
        console.log(bookList)
        return (
            <div>
                {/*<CarHead />*/}
                <TopBar/>
                <div id="chekoutBody">
                    <div className="checkoutProduct t">
                        <div className="checkoutProductInfo c">
                            <div className="checkoutTable">
                                {/*<input ></input>*/}
                                <ul className="checkoutTableHeader">
                                    <li>全选</li>
                                    <li>商品信息</li>
                                    <li>单价（元）</li>
                                    <li>数量</li>
                                    <li>小计（元）</li>
                                    <li>操作</li>
                                </ul>

                                <div className="checkoutProductBody">
                                    <div className="productBodyItem">
                                        <CarList info={bookList}/>
                                        {/*<CarList/>*/}
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div id="checkSubmit" className="t">
                        <div className="checkSubmitInfo c">
                            <div className="checkSubmitInfoBooking">
                                <div className="checkSubmitInfoRight r">
                                    <ul>
                                        <li><span>已选{this.state.bookNum}件商品，合计<span className="pri" >￥{this.state.sum}</span></span></li>
                                        <li>
                                            <span>商品总额：<span className="pri" >￥{this.state.sum}</span></span>
                                            {/*<div className="rightSave"><i></i><span>共节省：￥0.00</span></div>*/}
                                        </li>
                                    </ul>
                                    <div className="rightSubmit r">
                                        <Link to="/userCenter" onClick={this.createOrder.bind(this)}>
                                        立即结算
                                        </Link>

                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <Footer/>
                </div>
            </div>
        )
    }
}

