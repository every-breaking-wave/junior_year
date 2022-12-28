import React from "react";
import '../../css/base.css'
import '../../css/shoppingCar.css'
import '../../services/shoppingCarService'
import {addCart, changeCart, delCart, minCart} from "../../services/shoppingCarService";
import * as util from "util";
import {InputNumber,Checkbox} from "antd";
import Pubsub from "pubsub-js";
import {Link} from "react-router-dom";


export default class BookInCar extends React.Component{
    constructor(props) {
        super(props);
        this.state  = {utils: 1,num:1}
        this.minValue = this.minValue.bind(this)
        this.addValue = this.addValue.bind(this)
        const {info, deleteCartItem} = this.props;

        // this.handleDelete = this.handleDelete.bind(this)
    }
    onChange(value) {
        console.log('changed', value);
        changeCart(value,this.props.info.bookId)
        this.setState({num:value})
    }
    addValue(){
        addCart(this.props.info.id)
        this.setState({utils: !util})
        console.log(this.state.utils)
    }
    minValue(){
        if(this.props.info.number == 1) {
            alert("书本数量最少为1");
        }
        else {
            minCart(this.props.info.id)
            this.setState({utils: !util})
            console.log(this.state.utils)
        }
    }

    changeNum = (e)=>{
        if(e.target.value <= 0) alert("书本数量最少为1")
        else {
            this.props.getNum(e.target.value)
            this.setState({
                num: e.target.value
            })
        }
    }

    handleDelete(bookId){
        console.log("delete")
        delCart(bookId)
        Pubsub.publish('delCartItem',{isFirst:false,isLoading:true})
        this.props.deleteCartItem()
    }

    componentWillMount() {
        const {info} = this.props
        this.setState({
            num: info.number
        } )
        console.log(info)
    }



    render() {
        console.log(this.props)
        const bookId = this.props.info.book.id
        const {info} = this.props;

        // const {delete} = this.props
        return(
            <div onClick={this.hide} className="productBodyItemContent">
                <Checkbox defaultChecked={false} diabled />
                <Link className="itemImg" Link to={`/book/${info.book.id}`}>
                    <img src={info.book.cover} alt="" className={"bookInCar"}/>
                </Link>
                <div className="itemInfo">
                    <h5>《{info.book.bookName}》</h5>
                </div>
                <div className="itemInfoUnion1">
                    <span className="itemInfoUnionPice">{info.book.price}</span>
                </div>
                <div className="itemInfoUnion2">
                    <div className="itemInfoUnion2Module l">
                        <InputNumber min={1} max={10} defaultValue={this.state.num} onChange={this.onChange.bind(this)} size={"normal"}/>
                    </div>
                </div>
                <div className="itemInfoUnion3">
                    <span className="sumNum"> {info.book.price * 10 * this.state.num / 10} </span>
                </div>
                <div className="itemInfoUnion4">
                    <a onClick={this.handleDelete.bind(this,bookId)} className="delete"> </a>
                </div>
            </div>
        )
    }
}
