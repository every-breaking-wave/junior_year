import React from 'react';
import {Layout} from 'antd'
import BookList from "../components/BookList";
import HeaderL from "../components/Header/Header";
import Footer from "../components/Footer";
import '../css/list.css'
import '../css/base.css'
import '../css/home.css'
import axios from "axios";
import PubSub from "pubsub-js";
import {history} from "../utils/history";
import {createWebSocket, websocket} from "../Websocket";
import cookie from "react-cookies";


class HomeView extends React.Component {


    constructor(props) {
        super(props);
    }

    componentDidMount() {
        console.log(this.props)
        const {keyValue} = this.props.match.params
        console.log(keyValue)
        if (keyValue == "default") {
            axios.post(`/api/book/search/default`).then(
                response => {
                    console.log("请求成功", response.data);
                    if (response.data.bookList != null) {
                        PubSub.publish('searchBook', {isLoading: false, bookList: response.data.bookList})
                        history.push({pathname: '/default'})
                    }
                },
                error => {
                    PubSub.publish('searchBook', {err: error.message})
                }
            )
        } else {
            axios.post(`/api/book/search/${keyValue}`).then(
                response => {
                    console.log("请求成功", response.data);
                    if (response.data.bookList != null) {
                        PubSub.publish('searchBook', {isLoading: false, bookList: response.data.bookList})
                        history.push(`/${keyValue}`)
                    }
                },
                error => {
                    PubSub.publish('searchBook', {err: error.message})
                }
            )
        }
    }

    render() {
        return (
            <Layout>
                <div style={{background: "white"}}>
                    <div>
                        <HeaderL/>
                    </div>

                    <div id="productCategory" className="t">
                        <div className="brandNav c" style={{border: 20}}>
                        </div>

                        <div>
                            <BookList/>
                        </div>
                    </div>
                    <Footer/>
                </div>

            </Layout>);
    }
}


export default HomeView;

