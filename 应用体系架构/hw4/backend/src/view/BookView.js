import React from 'react';
import {Layout} from "antd";
import '../css/base.css'
import '../css/detail.css'
import '../css/FixedRight.css'
import HeaderL from "../components/Header/Header";
import Footer from "../components/Footer";
import BookDetail from "../components/BookDetail/BookDetail";
import {getBook} from "../services/bookService";

const { Content } = Layout;

class BookView extends React.Component{

    constructor(props) {
        super(props);
         this.state = {book:null};
         this.componentDidMount = this.componentDidMount.bind(this)
    }

    componentDidMount(){

        const callback = (book) => {
            this.setState({book: book});
            console.log(this.state)
        };

        console.log(this.props)
        getBook(this.props.match.params.id, callback);
    }

    searchBooks = (bookName)=>{
        console.log('bookView'+bookName);
    }

    render(){
        const {book} = this.state;
        return(
           <div style={{background:"white"}}>
               <Layout className="layout">
                    <div>
                   <HeaderL searchBooks = {this.searchBooks}/>
                    </div>
                   <Layout>
                       <Content style={{ padding: '0 50px' }}>
                           <div className="home-content">
                               <BookDetail info={book}/>
                               <div className={"foot-wrapper"}>
                               </div>
                           </div>
                       </Content>
                   </Layout>
               </Layout>
                <Footer/>
           </div>
        );
    }
}

export default BookView;

