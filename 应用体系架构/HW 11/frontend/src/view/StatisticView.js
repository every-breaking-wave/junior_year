import React, {Component} from 'react';
import axios from "axios";
import {message,Button, Descriptions, DatePicker} from "antd";
import {compare, getOrderItemsById} from "../services/orderService";
import Footer from "../components/Footer";
import TopBar from "../components/Header/TopBar";


const {RangePicker} = DatePicker;

export default class StatisticView extends Component {
    constructor(props) {
        super(props);
        this.state = {sum: 0}
        this.state = {orderList: [], allOrders: [], status: 0, bookList: [], userList: [], allUsers: []}
    }

    componentDidMount() {
        axios.post(`/api/admin/get-full-order`).then(
            response => {
                console.log("请求成功", response.data);
                if (response.data != null) {
                    // history.push({pathname: '/default'})
                    this.setState({orderList: response.data})
                    this.setState({allOrders: response.data})
                    this.calculateOrderSum(response.data)
                }
            },
        )
    }

    timeChange = (value, dateString) => {
        if (this.state.status == 0 || dateString[0] === '' || dateString[1] === '') {
            return;
        }
        dateString[0] = dateString[0].replace(/-/g, '/');
        dateString[1] = dateString[1].replace(/-/g, '/');
        console.log(dateString[0], dateString[1])
        const startTime = new Date(Date.parse(dateString[0]));
        const endTime = new Date(Date.parse(dateString[1]));
        let data = this.state.allOrders;
        let number = new Array(40);
        let bookNames = new Array(40);
        let sum = new Array(40);
        let authors = new Array((40))
        let users = new Array(40);
        let arr = [];
        let sumCount = 0;
        let numberCount = 0;
        for (let index = 0; index < 40; index++) {
            number[index] = 0;
            bookNames[index] = '';
            sum[index] = 0;
        }
        console.log(data)
        if (this.state.status == 1) {
            console.log('Formatted Selected Time: ', dateString);

            for (let i = 0; i <= data.length - 1; i++) {
                let time = new Date(Date.parse(data[i].createTime));
                if (
                    time > startTime && time <= endTime
                ) {
                    for (let j = 0; j <= data[i].orderItems.length - 1; j++) {
                        let item = data[i].orderItems[j];
                        let book = item.book;
                        let bookId = book.id;
                        console.log("reach")
                        number[bookId]++;
                        bookNames[bookId] = book.bookName;
                        authors[bookId] = book.author
                        sum[bookId] += Number((item.num * item.price));
                    }
                }
            }
            for (let index = 0; index < 40; index++) {
                if (number[index] !== 0) {
                    let json = {
                        bookId: index,
                        author: authors[index],
                        bookName: bookNames[index],
                        bookNumber: number[index],
                        sum: sum[index].toFixed(1)
                    };
                    sumCount += sum[index];
                    numberCount += number[index]
                    arr.push(json)
                }
            }
            this.setState({
                arr: arr, number: numberCount, sum: sumCount, status: 1
            })
            console.log(arr)
        }

    }


    handlePanelClick = async (userId, index) => {
        const orderItemsRes = await getOrderItemsById(userId)
        console.log(orderItemsRes)
        this.setState({orderItems: orderItemsRes})
    }

    callback = (bookInCarList) => {
        this.setState({orderList: bookInCarList})
        console.log(this.state.orderList)
    }

    anaBooks = () => {
        this.setState({status: 1})
        message.info("按照书本统计")
    }

    anaUsers = () => {
        this.setState({status: 2})
        message.info("按照用户统计")

    }




    findUser(id, userArr) {
        let i
        for (i = 0; i < userArr.length; i++) {
            if (userArr[i] == id) {
                return i
            }
        }
    }

    handleAnaBook(dateString) {
        const startTime = new Date(Date.parse(dateString[0]));
        const endTime = new Date(Date.parse(dateString[1]));
        let data = this.state.allOrders;
        let number = new Array(40);
        let bookName = new Array(40);
        let sum = new Array(40);
        let author = new Array((40))
        let arr = [];
        for (let index = 0; index < 40; index++) {
            number[index] = 0;
            bookName[index] = '';
            sum[index] = 0;
        }
        // debugger;
        for (let i = 0; i <= data.length - 1; i++) {
            let time = new Date(Date.parse(data[i].createTime));
            if (
                time > startTime && time < endTime
            ) {
                for (let j = 0; j <= data[i].orderItems.length - 1; j++) {
                    let item = data[i].orderItems[j];
                    let book = item.book;
                    let bookId = book.id;
                    author[bookId] = book.author
                    number[bookId] += item.number;
                    bookName[bookId] = book.bookName;
                    sum[bookId] += Number((item.number * item.price));
                }
            }
        }
        // debugger;
        for (let index = 0; index < 40; index++) {
            if (number[index] !== 0) {
                let json = {
                    bookId: index,
                    author: author[index],
                    bookName: bookName[index],
                    bookNumber: number[index],
                    sum: sum[index].toFixed(1)
                };
                arr.push(json)
            }
        }
        console.log(arr)
        arr.sort(compare('bookNumber'))
        console.log(arr)

        this.setState({
            bookList: arr
        })
    }

    handleAnaUser(dateString) {
        const startTime = new Date(Date.parse(dateString[0]));
        const endTime = new Date(Date.parse(dateString[1]));
        let data = this.state.allOrders;
        let number = new Array(40);
        let sum = new Array(40);
        let userArr = new Array(100)
        let arr = [];

        for (let i = 0; i < data.length; i++) {
            let userId = data[i].userId
            userArr.push(userId)
        }

        for (let i = 0; i < userArr.length; i++) {
            for (let j = i + 1; j < userArr.length; j++) {
                if (userArr[i] == userArr[j]) {         //去除重复用户
                    userArr.splice(j, 1);
                    j--;
                }
            }
        }

        for (let index = 0; index < userArr.length; index++) {
            number[index] = 0;
            sum[index] = 0;
        }

        for (let i = 0; i <= data.length - 1; i++) {
            let time = new Date(Date.parse(data[i].createTime));
            let userId = data[i].userId
            let userArrIndex
            console.log(userArr)
            userArrIndex = this.findUser(userId, userArr)
            console.log(userArrIndex)
            if (
                time > startTime && time < endTime
            ) {
                for (let j = 0; j <= data[i].orderItems.length - 1; j++) {
                    let item = data[i].orderItems[j];
                    number[userArrIndex]++;
                    sum[userArrIndex] += Number((item.number * item.price));
                }
            }
        }
        // debugger;
        for (let index = 0; index < userArr.length; index++) {
            let json = {
                userId: userArr[index],
                bookNumber: number[index],
                sum: sum[index].toFixed(1)
            };
            if (json.bookNumber !== 0)
                arr.push(json)
        }
        console.log(arr)
        arr.sort(compare('sum'))
        console.log(arr)
        for (let j = 0; j < userArr.length; j++) {
            if (typeof userArr[j] == "undefined") {
                userArr.splice(j, 1)
            }
        }
        this.setState({
            userList: arr
        })
    }

    timeChange = (value, dateString) => {
        if (this.state.status == 0 || dateString[0] === '' || dateString[1] === '') {
            return;
        }
        if (this.state.status == 1) {
            this.handleAnaBook(dateString)
        } else this.handleAnaUser(dateString)
        console.log('Formatted Selected Time: ', dateString);

    }


    render() {
        const userList = this.state.userList || []
        const bookList = this.state.bookList || []
        let date
        return (
            <div>
                <TopBar/>
                <Button onClick={this.anaBooks}> 统计书本 </Button>
                <Button onClick={this.anaUsers}> 统计用户 </Button>
                <RangePicker onChange={this.timeChange}/>
                {
                    this.state.status == 0 ?
                        <div></div>
                        : this.state.status == 1 ?

                            bookList && bookList.map((book, index) => {
                                return (
                                    <Descriptions column={4}>
                                        <Descriptions.Item label="书名">
                                            《{book.bookName}》
                                        </Descriptions.Item>
                                        <Descriptions.Item label="作者">
                                            {book.author}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="总价">
                                            {book.sum}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="总销量">
                                            {book.bookNumber}
                                        </Descriptions.Item>
                                    </Descriptions>
                                )
                            })
                            :
                            userList && userList.map((user, index) => {
                                return (
                                    <Descriptions column={3}>
                                        <Descriptions.Item label="用户ID">
                                            {user.userId}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="购买数量">
                                            {user.bookNumber}
                                        </Descriptions.Item>
                                        <Descriptions.Item label="总花费">
                                            {user.sum}
                                        </Descriptions.Item>
                                    </Descriptions>
                                )
                            })
                }
                <Footer/>
            </div>
        );
    }


}

