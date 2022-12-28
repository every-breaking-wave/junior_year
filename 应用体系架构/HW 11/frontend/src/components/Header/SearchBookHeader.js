import React from "react";
import '../../css/header.css'
import '../../css/base.css'
import {getRole, logout} from "../../services/userService";
import TopBar from "./TopBar";
import SearchBook from "./SearchBook";
import SearchOrder from "./SearchOrder";

class SearchBookHeader extends React.Component {

    constructor(props) {
        super(props);
        this.state = {list: [], role: false};
        this.search = this.search.bind(this);
    }

    search = (e) => {
        var arr = this.state.list.filter((item, index) => item.name.indexOf(e.target.value) > -1);
        this.setState({list: arr});
    };

    async componentDidMount() {
        const userRole = await getRole();
        this.setState({role: userRole})
        console.log(this.state.role)
        console.log({userRole})
        if (typeof this.state.role == "undefined") {
            this.setState({role: false})
        }
        console.log(this.state.role)
    }

    render() {
        return (
            <div>
                <TopBar/>
                <SearchBook/>
            </div>
        )
    }
}

export default SearchBookHeader



