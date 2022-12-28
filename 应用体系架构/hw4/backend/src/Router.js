import React from 'react';
import { Router, Route, Switch, Redirect,BrowserRouter, HashRouter, Link } from 'react-router-dom';
import LoginRoute from './LoginRoute'
import PrivateRoute from "./PrivateRoute";
import HomeView from "./view/HomeView";
import LoginView from './view/LoginView'
import BookView from "./view/BookView";
import RegisterView from "./view/RegisterView"
import { history } from "./utils/history";
import ShoppingCarView from "./view/ShoppingCarView";
import UserManagerView from "./view/UserManageView"
import OrderManageView from "./view/OrderManageView";
import UserView from "./view/UserView"
import BookManageView from "./view/BookManageView";
import StatisticView from "./view/StatisticView";



// TODO: 尚未实现前端鉴权
class BasicRoute extends React.Component {
    constructor(props) {
        super(props);

        history.listen((location, action) => {
            // clear alert on location change
            console.log(location, action);
        });
    }

    render() {
        return (
            <div>
                <BrowserRouter >
                    <Switch>
                        <Route  path="/login" component={LoginView} />
                        <Route  path="/register" component={RegisterView} />
                        <LoginRoute path="/book/:id" component={BookView} />
                        <LoginRoute path="/car" component={ShoppingCarView} />
                        <PrivateRoute path="/bookManage/:keyValue" component={BookManageView} />
                        <PrivateRoute path="/userManage" component={UserManagerView} />
                        <PrivateRoute path="/orderManage/:keyValue" component={OrderManageView} />
                        <LoginRoute path="/userCenter" component={UserView} />
                        <PrivateRoute path="/statistic" component={StatisticView} />
                        <LoginRoute path="/:keyValue" component={HomeView} />
                        <Redirect from="/bookManage/*" to="/bookManage/default" />
                        <Redirect from="/orderManage/*" to="/orderManage/default" />
                        <Redirect from="/userCenter/*" to="/userCenter/default" />
                        <Redirect from="/*" to="/default" />
                    </Switch>
                </BrowserRouter>
            </div>

        )
    }
}

export default BasicRoute;
