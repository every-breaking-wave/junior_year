import React, {Component} from 'react';
import BookInCar from "./BookInCar";
import {getCar} from "../../services/shoppingCarService";
import PubSub from "pubsub-js";

class CarList extends Component {
    constructor(props) {
        super(props);
        this.state = {sum:0}
        this.state = {bookList:[]}
    }

    callback=(bookInCarList)=>{
        this.setState({bookList:bookInCarList})
        console.log(this.state.bookList)

    }

    delete () {
        console.log("reach here")
        getCar(this.callback)
    }


    componentDidMount(){
        PubSub.subscribe('delCartItem',(_,stateObj)=>{
            console.log('收到书本数据!',stateObj);
            this.setState(stateObj)
        })
       getCar(this.callback)
    }

    render() {
        const bookList = this.state.bookList || []
        console.log(bookList)
        return (
            <div>
                {
                    bookList.map((item,index)=>
                        <div>
                            <BookInCar key={item+index} info={item} deleteCartItem={this.delete}/>
                        </div>
                    )
                }
            </div>
        );
    }
}

export default CarList;
