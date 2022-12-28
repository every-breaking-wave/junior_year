// 单行可编辑
import React from "react";
import {Table, Input, Button} from "antd";
import Footer from "../components/Footer";
import axios from "axios";
import Pubsub from "pubsub-js";
import {delBook, refreshBook} from "../services/bookService";
import SearchBookHeader from "../components/Header/SearchBookHeader";


export default class BookManageView extends React.Component {
    state = {
        dataSource: [],
        editArr: [],
    };



     componentDidMount() {
             axios.post(`/api/book/search/default`).then(
                 response => {
                     console.log("请求成功", response.data);
                     if(response.data.bookList != null){
                         this.setState({dataSource : response.data.bookList})
                     }
                 },
             )

         Pubsub.subscribe('searchBook1',(_,stateObj)=>{
             console.log('收到书本数据!',stateObj);
             this.setState({dataSource : stateObj.bookList})
             console.log(this.state.dataSource)
         })
     }

    // 渲染出来input,输入的时候改变dataSource的数据
    renderInput = (text, record, index, field) => {
        const {editArr} = this.state;
        return record.edit ? (
            <Input
                value={
                    editArr[index] && editArr[index][field]
                        ? editArr[index][field]
                        : record[field]
                }
                onChange={(e) => this.inputChange(e, record, index, field)}
            />
        ) : (
            text
        );
    };

    // 编辑表格
    edit = (record, id) => {
        const {dataSource} = this.state;
        // 浅拷贝下数据
        const newArr = [...dataSource];
        // 找到index的值
        const index = newArr.findIndex((item) => item.id === id);
        // 利用splice方法删除原数据，新增新数据
        newArr.splice(index, 1, {...record, edit: true});
        // 注意：一开始写法是const arr = newArr.splice(index, 1, { ...record, ...{ edit: true } });是错的，因为这个方法返回的是删除的那一条值
        this.setState({dataSource: newArr});
    };

    // 新增书籍
    addBook = () => {
        const {dataSource} = this.state;
        const arr = dataSource.map((item) => {
            return {
                ...item,
                edit: false,
            };
        });
        this.setState({dataSource: arr}, () => {
            console.log(dataSource, "--dataSource");
        });
    };

    // input改变的时候
    inputChange = (e, record, index, field) => {
        let { editArr } = this.state;
        editArr[index] = record;
        record[field] = e.target.value;
        this.setState({ editArr });
    };

    // 一键保存所有数据
    saveAll = () => {
        const {dataSource} = this.state;
        const arr = dataSource.map((item) => {
            return {
                ...item,
                edit: false,
            };
        });
        this.setState({dataSource: arr}, () => {
            console.log(dataSource, "--dataSource");
        });
    };

    // 单条保存
    handleSave = (record, index) => {
        console.log(index)
        const {editArr, dataSource} = this.state;

        const newData = [...dataSource];
         index = newData.findIndex((item) => item.id === index);
        // 用splice不改变原来的数组顺序
        newData.splice(index, 1, {
            ...record,
            ...editArr[index],
            edit: false,
        });
        this.setState({dataSource: newData});
        refreshBook(record)
        console.log(this.state.dataSource)
    };

    // 单条保存
    handleDelete = ( index) => {
        console.log(index)
        const {dataSource} = this.state;
        const newData = [...dataSource];
        const id = newData.findIndex((item) => item.id === index);
        // 利用splice方法删除原数据，新增新数据
        newData.splice(id, 1, {edit: false});
        this.setState({dataSource: newData});
        delBook(dataSource[id])
        console.log(dataSource[index])
        console.log(this.state.dataSource)
    };

    // 新增row
    handleAdd = () => {
        const {dataSource} = this.state;
        const key = new Date().toString();
        const row = {
            key,
            bookName: '',
            author: '',
            price: '',
            bookDesciption: '',
            inventory: ''
        };
        console.log(dataSource);
        console.log(row);
        const newData = dataSource;
        newData.splice(0, 0, {...row ,edit:true});
        this.setState({dataSource: newData});
        console.log(newData);
    }

        render() {
        // const books = this.state.dataSource
        console.log(this.state.dataSource)
        const columns = [
            {
                title: "封面",
                dataIndex: "cover",
                key: "cover",
                render: (text) => (
                        <img src={text}  style={{ width: 100 }}/>
                )
            },
            {
                title: "书名",
                dataIndex: "bookName",
                key: "bookName",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "bookName"),
            },
            {
                title: "分类",
                dataIndex: "category",
                key: "category",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "category"),
            },
            {
                title: "作者",
                dataIndex: "author",
                key: "author",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "author"),
            },
            {
                title: "价格",
                dataIndex: "price",
                key: "price",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "price"),
            },
            {
                title: "简介",
                dataIndex: "bookDescription",
                key: "bookDescription",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "bookDescription"),
            },
            {
                title: "库存",
                dataIndex: "inventory",
                key: "inventory",
                render: (text, record, index) =>
                    this.renderInput(text, record, index, "inventory"),
            },
            {
                title: "操作",
                dataIndex: "edit",
                key: "edit",
                render: (text, record, index) => {
                    return !record.edit ? (
                        <span
                            style={{color: "black", cursor: "pointer"}}
                            onClick={() => this.edit(record, record.id)}
                        >
              编辑
            </span>
                    ) : (
                        <span
                            style={{color: "blue", cursor: "pointer"}}
                            onClick={() => this.handleSave(record, record.id)}
                        >
              保存
            </span>
                    );
                },
            },
            {
                title: "操作",
                dataIndex: "delete",
                key: "delete",
                render: (text, record, index) => {
                    return(
                        <span
                            style={{color: "red", cursor: "pointer"}}
                            onClick={() => this.handleDelete(record.id)}
                        >
                          删除
                        </span>
                    )
                },
            },
        ];

        return (
            <div>
                <SearchBookHeader/>
                <Button type="primary" onClick={this.handleAdd} >
                    新增书籍
                </Button>
                <div style={{width: "100%"}}>

                    {
                        this.state.dataSource  ? (
                                <Table
                                    rowKey={(record) => record.id}
                                    dataSource={this.state.dataSource}
                                    columns={columns}

                                />
                        ) : (
                            <div></div>
                        )
                    }
                    {/*<Button type="primary" onClick={this.saveAll}>*/}
                    {/*    一键保存（还未实现)*/}
                    {/*</Button>*/}

                </div>
                <Footer/>
            </div>

        );
    }
}