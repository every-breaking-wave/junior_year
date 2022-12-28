import React, {Component} from 'react';
import '../../css/header.css'
import '../../css/home.css'
import {Input, message, Switch} from "antd";
import Pubsub from 'pubsub-js'
import axios from 'axios';
import { Link } from 'react-router-dom';
import {history} from "../../utils/history";
const {SearchInput} = Input
class Search extends Component {

    constructor(props) {
        super(props);
        // this.state  = {keyValue:"未输入"}
        this.handleSearch = this.handleSearch.bind(this)
        this.onClickGotoCart = this.onClickGotoCart.bind(this)
        this.changeSearchMode = this.changeSearchMode.bind(this)
        this.state = {fulltext : false};
    }

    onClickGotoCart() {
        history.push({ pathname: '/car' })    }

    changeSearchMode(){
        this.setState({fulltext : !this.state.fulltext})
    }

    handleSearch(){
        const keyword = this.keyWordElement.value
        console.log("keyword is " + keyword)
        Pubsub.publish('searchBook',{isFirst:false,isLoading:true})
        if(this.state.fulltext) {
            axios.post(`/api/book/fulltextsearch/${keyword}`).then(
                response => {
                    console.log("请求成功", response.data);
                    if(response.data.bookList != null){
                        Pubsub.publish('searchBook',{isLoading:false, bookList: response.data.bookList})
                        message.info("全文搜索成功！")
                        history.push(`/${keyword}`)
                    }
                },
                error => {
                    Pubsub.publish('searchBook',{err: error.message})
                }
            )
        }
        if(keyword == ""){
            axios.post(`/api/book/search/default`).then(
                response => {
                    console.log("请求成功", response.data);
                        if(response.data.bookList != null){
                            Pubsub.publish('searchBook',{isLoading:false, bookList: response.data.bookList})
                            history.push({ pathname: '/default' })
                        }
                },
                error => {
                    Pubsub.publish('searchBook',{err: error.message})
                 }
            )
            // message.error("请输入搜索关键字")
        }
        else{
            axios.post(`/api/book/search/${keyword}`).then(
                response => {
                    console.log("请求成功", response.data);
                        if(response.data.bookList != null){
                            Pubsub.publish('searchBook',{isLoading:false, bookList: response.data.bookList})
                            history.push(`/${keyword}`)
                        }
                },
                error => {
                    Pubsub.publish('searchBook',{err: error.message})
                 }
            )
        }
    }

    render() {
        return (

            <div id="search" className="t">
            <div id="searchInfo" className="c">
                <div className="searchIContent c">
                    <div className="searchBox l" style={{marginLeft:200}}>
                        {/*search功能*/}
                        <div className="searchBoxMain" >
                            <div className="searchBoxInput l">
                                <input ref={c => this.keyWordElement = c} type="text"
                                       placeholder="搜索你想要的书目"/>&nbsp;
                            </div>
                            <a onClick={this.handleSearch} className="searchButton">搜索</a>
                        </div>
                    </div>
                    <Switch style={{marginTop: 10}} checkedChildren="全文搜索" unCheckedChildren="关闭" defaultChecked={false} onClick={this.changeSearchMode}/>

                    <div className="searchIContentBag r" >
                        <div className="searchIContentBag01 r">
                            <Link to={"/car"}>
                                <em className="bagIcon"></em>
                                购物车
                            </Link>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        );
    }
}

export default Search;
