import React from 'react';
import {Route, Redirect} from 'react-router-dom'
import {message} from "antd";
import './services/userService'
import {getRole, loginUser} from "./services/userService";

export default class PrivateRoute extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            role: null,
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


    async componentDidMount() {
        if (!loginUser())
            message.info("你需要先进行登录")
        let role = await getRole()
        this.setState({role : role})
        if (!role) {
            console.log(role)
            message.info("你没有此权限")
        }
    }


      render() {


        const {component: Component, path = "/", exact = false, strict = false} = this.props;

        let role = this.state.role

         console.log("role "+  role)

        return <Route path={path} exact={exact} strict={strict} render={props => (
            !loginUser() ? (
                <Redirect to={{
                    pathname: '/login',
                    state: {from: props.location},
                    message
                }}/>
            ) : role == false ? (

                    <Redirect to={{
                        pathname: '/',
                        state: {from: props.location},
                        message
                    }}/>)
                : (
                    <Component {...props}/>
                )

        )}/>

    }
}

