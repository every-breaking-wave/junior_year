import React from "react";
import '../css/header.css'
import '../css/base.css'
import '../css/footer.css'

export default class Footer extends React.Component{

    render() {
        return (
        <div>
            <div className="foot t">
                <div className="footContent c">
                    <ul className="footList">
                        <li>
                            {/*<img src={info.cover} alt="" className={"bookInCar"}/>*/}
                            <a  href="https://github.com/">Wave's GitHub</a>
                            {/*<a href="">ICP备案序号:沪ICP备xxxxxxxx号</a>*/}
                        </li>
                    </ul>
                </div>
            </div>
        </div>
        )
    }
}
