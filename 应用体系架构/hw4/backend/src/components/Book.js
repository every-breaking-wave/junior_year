import React from 'react';
import {Card, message} from 'antd';
import { Link } from 'react-router-dom'
const { Meta } = Card;

export default class Book extends React.Component {

    render() {

        const { info } = this.props;

        const gridStyle = {
            width: '100%',
            textAlign: 'center',
        };

        return (
            <Link to={`/book/${info.id}`}>

                <Card
                    hoverable
                    style={{ width: 181, height: 250 }}
                    cover={
                        <img
                             className={"bookImg"}
                            src={info.cover}
                            style={{ width: 100, marginLeft: 40, height: 100 }}
                        />
                    }
                    // onClick={handleBookClick}
                >
                    <Meta
                        title= {info.bookName}
                        // description={info.bookDescription}
                        style={gridStyle}
                    />
                    <p
                        style={gridStyle}
                    >
                        作者:{info.author}
                    </p>
                    <p style={gridStyle} >价格：{info.price}元</p>
                </Card>


            </Link>
        );
    }

}

