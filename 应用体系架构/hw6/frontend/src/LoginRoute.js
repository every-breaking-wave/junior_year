import React from 'react';
import {Route, Redirect} from 'react-router-dom'
import * as userService from "./services/userService"
import {message} from "antd";
import './services/userService'
import {loginUser} from "./services/userService";

export default class LoginRoute extends React.Component{
    constructor(props) {
        super(props);
        this.state = {
            isAuthed: false,
            hasAuthed: false,
        };
    }

    checkAuth = (data) => {
        console.log(data);
        if (data.status >= 0) {
            this.setState({isAuthed: true, hasAuthed: true});
        } else {
            message.error(data.msg);
            localStorage.removeItem('user');
            this.setState({isAuthed: false, hasAuthed: true});
        }
    };


    componentDidMount() {
        if(!loginUser())
            message.info("你需要先进行登录")
        // userService.checkSession(this.checkAuth);
    }


    render() {


        const {component: Component, path="/",exact=false,strict=false} = this.props;

        return <Route path={path} exact={exact} strict={strict} render={props => (
           loginUser() ? (
                <Component {...props}/>
            ) : (
                <Redirect to={{
                    pathname: '/login',
                    state: {from: props.location},
                    message
                }}/>
            )
        )}/>

    }
}

