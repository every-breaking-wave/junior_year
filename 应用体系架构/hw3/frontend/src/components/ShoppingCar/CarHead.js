import React from "react";
import '../../css/base.css'
import '../../css/shoppingCar.css'


export default class CarHead extends React.Component{


    render() {
        return(
    <div>
    <div id="headTop" className="t">
                <div id="topContent" className="c">
				<span className="topContentLeft l">
					<span className="a1">欢迎来到EBook</span>
					<a className="pleaseLogin" href="#">184****2829</a>
					<em></em>
					<li></li>
					<a className="pleaseregister" href="list.html">退出</a>
				</span>
                    <span className="topContentRight r">
					<span>
						<span className="icon1"></span>
						<a href="#" className="a1">我的EBook</a>
						<span className="icon2"></span>
						<ul className="icon2Hover">
							<li>
								<a href="#">我的订单</a>
							</li>
						</ul>
					</span>
					<span>
						<span className="icon3"></span>
						<a href="#" className="a1">会员俱乐部</a>
					</span>
					<span>
						<span className="icon4"></span>
						<a href="#" className="a1">手机书城</a>
					</span>
					<span>
						<a href="#">帮助中心</a>
					</span>
				</span>
                </div>
            </div>
        <div id="carHeader">
            <div className="carHeaderInfo t">
                <div className="carHeaderInfoContent c">
                    <div className="carHeaderContentLeft l">
                        <a href="list.html"><img src= {require("../../assets/header/logo.png")} width="320"/></a>
                    </div>
                    <div className="carHeaderContentRight r">
                        <div className="carHeaderContentRightBorder"></div>
                        <div className="carHeaderRightStep l carHeaderRightStep1">
                            <em className="checkoutStep"></em>
                        </div>
                        <div className="carHeaderRightStep l carHeaderRightStep2">
                            <em className="checkoutStep"></em>
                        </div>
                        <div className="carHeaderRightStep l carHeaderRightStep3">
                            <em className="checkoutStep"></em>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>

        )
    }
}
