import React from 'react';
import {List} from 'antd';
import Pubsub from 'pubsub-js'
import Book from './Book'
import '../css/detail.css'



export default class BookList extends React.Component{

    
    constructor(props) {
        super(props);
    }

    state = {
        bookList:[],
         isFirst:true,
         isLoading:false,
         err:''
    }

    componentDidMount(){
        console.log("props = "+this.props)
        Pubsub.subscribe('searchBook',(_,stateObj)=>{
            console.log('收到书本数据!',stateObj);
            this.setState(stateObj)
        })
    }

    render() {
        const {bookList, isFirst, isLoading,err} = this.state;

        return (

            <List style={{marginLeft:100 ,marginTop:20}}
                grid={{gutter: 10, column: 4}}
                dataSource={this.state.bookList}
                pagination={{
                    onChange: page => {
                        console.log(page);
                    },
                    pageSize: 16,
                }}

                renderItem={item => (
                    <List.Item>
                        <Book info={item} />
                    </List.Item>
                )}
            />
        );
    }
}


