import React, {Component} from "react";

export default class LeftNav extends React.Component{

    render() {
        const {cato, cato1,cato2,cato3,cato4,cato5,cato6} = this.props;
        return(
            <li>
                <span className="border r"></span>
                <a href="#" className="list">{cato}</a>
                <div className="navInfoContentHover">
                    <div className="navHoverInfo l">
                        <div className="navHoverInfoTi">
                            <span>热门</span>
                        </div>
                        <div className="navHoverInfo1">
                            <ul>
                                <a href="#">{cato1}</a><a href="#">{cato4}</a>
                                <a href="#">{cato2}</a><a href="#">{cato5}</a>
                                <a href="#">{cato3}</a><a href="#">{cato6}</a>
                            </ul>
                            <ul>
                                <a href="#">{cato1}</a><a href="#">{cato4}</a>
                                <a href="#">{cato2}</a><a href="#">{cato5}</a>
                                <a href="#">{cato3}</a><a href="#">{cato6}</a>
                            </ul>
                            <ul>
                                <a href="#">{cato1}</a><a href="#">{cato4}</a>
                                <a href="#">{cato2}</a><a href="#">{cato5}</a>
                                <a href="#">{cato3}</a><a href="#">{cato6}</a>
                            </ul>
                            <ul>
                                <a href="#">{cato1}</a><a href="#">{cato4}</a>
                                <a href="#">{cato2}</a><a href="#">{cato5}</a>
                                <a href="#">{cato3}</a><a href="#">{cato6}</a>
                            </ul>
                        </div>
                    </div>
                </div>
            </li>
        )
    }
}


