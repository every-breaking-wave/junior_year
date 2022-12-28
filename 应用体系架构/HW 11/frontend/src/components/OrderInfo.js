// // import React, { useState } from 'react';
// // import { Table, Radio, Divider } from 'antd';
// // const columns = [
// //     {
// //         title: 'OrderItemId',
// //         dataIndex: 'orderId',
// //         render: (text) => <a>{text}</a>,
// //     },
// //     {
// //         title: 'Num',
// //         dataIndex: 'num',
// //     },
// //     {
// //         title: 'Price',
// //         dataIndex: 'price',
// //     },
// // ];
// // // const data = [
// // //     {
// // //         key: '1',
// // //         name: 'John Brown',
// // //         age: 32,
// // //         address: 'New York No. 1 Lake Park',
// // //     },
// // //     {
// // //         key: '2',
// // //         name: 'Jim Green',
// // //         age: 42,
// // //         address: 'London No. 1 Lake Park',
// // //     },
// // //     {
// // //         key: '3',
// // //         name: 'Joe Black',
// // //         age: 32,
// // //         address: 'Sidney No. 1 Lake Park',
// // //     },
// // //     {
// // //         key: '4',
// // //         name: 'Disabled User',
// // //         age: 99,
// // //         address: 'Sidney No. 1 Lake Park',
// // //     },
// // // ]; // rowSelection object indicates the need for row selection
// //
// // const rowSelection = {
// //     onChange: (selectedRowKeys, selectedRows) => {
// //         console.log(`selectedRowKeys: ${selectedRowKeys}`, 'selectedRows: ', selectedRows);
// //     },
// //     getCheckboxProps: (record) => ({
// //         disabled: record.name === 'Disabled User',
// //         // Column configuration not to be checked
// //         name: record.name,
// //     }),
// // };
// //
// // const Demo = () => {
// //     const [selectionType, setSelectionType] = useState('checkbox');
// //     const {info} = this.props;
// //     return (
// //         <div>
// //             <Radio.Group
// //                 onChange={({ target: { value } }) => {
// //                     setSelectionType(value);
// //                 }}
// //                 value={selectionType}
// //             >
// //                 <Radio value="checkbox">Checkbox</Radio>
// //                 <Radio value="radio">radio</Radio>
// //             </Radio.Group>
// //
// //             <Divider />
// //
// //             <Table
// //                 rowSelection={{
// //                     type: selectionType,
// //                     ...rowSelection,
// //                 }}
// //                 columns={columns}
// //                 dataSource={info}
// //             />
// //         </div>
// //     );
// // };
// //
// // export default Demo;
//
//
// const columns = [
//     {
//         title: 'Id',
//         dataIndex: 'orderId',
//         sorter: {
//             compare: (a, b) => a.orderId - b.orderId,
//             multiple: 3,
//         },
//     },
//     {
//         title: 'Num',
//         dataIndex: 'num',
//         sorter: {
//             compare: (a, b) => a.num - b.num,
//             multiple: 2,
//         },
//     },
//     {
//         title: 'Price',
//         dataIndex: 'price',
//         sorter: {
//             compare: (a, b) => a.price - b.price,
//             multiple: 1,
//         },
//     },
// ];
//
//
//
//
// export default () => <Table columns={columns} dataSource={data} onChange={onChange} />;