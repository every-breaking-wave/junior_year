import React, {Component} from "react";

export default class NavList extends React.Component{

    render() {
        const {nav, books} = this.props;
        return(
            <li className="liRelative">
                <a href="#">{nav} </a>
                <div className="dropDown">
                    <div className="dropDownContentT l">
                        <a href="#">全部</a>
                    </div>
                    <ul className="l">
                        <li><a href="#">{books}</a></li>
                        <li><a href="#">{books}</a></li>
                        <li><a href="#">{books}</a></li>
                        <li><a href="#">{books}</a></li>
                        <li><a href="#">{books}</a></li>
                    </ul>
                </div>
            </li>
        )
    }
}


