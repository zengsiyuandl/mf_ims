const Http = require('http');
const Net = require('net');
const Buffer = require('buffer');

// 创建一个到 C 语言服务器的 TCP 客户端
const g_tcpClient = new Net.Socket();

const MSG_OBJ_TCP_HOST = '127.0.0.1';  // C 语言服务器的地址
const MSG_OBJ_TCP_PORT = 12001;  // C 语言服务器的端口

const MSG_OBJ_MAX_KEY_SIZE = 64;
const MSG_OBJ_MAX_VALUE_SIZE = 256;
const MSG_OBJ_MAX_NUM = 16;
const MSG_OBJ_UINT32T_SIZE = 4;

// 连接到 C 语言服务器
g_tcpClient.connect(MSG_OBJ_TCP_PORT, MSG_OBJ_TCP_HOST, () => {
    console.log('Connected to C language server via TCP');
});


function convertJsonToCStruct(json, objId, httpMethod) {

    let buffer = Buffer.alloc(MSG_OBJ_UINT32T_SIZE + MSG_OBJ_UINT32T_SIZE +
        (MSG_OBJ_MAX_KEY_SIZE + MSG_OBJ_MAX_VALUE_SIZE) * MSG_OBJ_MAX_NUM);
    let offset = 0;

    // 写入objId (uint32_t)
    buffer.writeUInt32LE(objId, offset);
    offset += MSG_OBJ_UINT32T_SIZE;

    // 写入httpMethod (uint32_t)
    buffer.writeUInt32LE(httpMethod, offset);
    offset += MSG_OBJ_UINT32T_SIZE;

    // 写入jsonObj数组
    const entries = Object.entries(json).slice(0, MSG_OBJ_MAX_NUM); // 限制最大处理数量
    entries.forEach(([key, value]) => {
        // 写入key
        buffer.write(key, offset, MSG_OBJ_MAX_KEY_SIZE, 'ascii');
        offset += MSG_OBJ_MAX_KEY_SIZE;

        // 写入value
        let stringValue = value.toString();
        buffer.write(stringValue, offset, MSG_OBJ_MAX_VALUE_SIZE, 'ascii');
        offset += MSG_OBJ_MAX_VALUE_SIZE;
    });

    return buffer;
}

// 现在，你可以通过TCP客户端发送这个buffer
g_tcpClient.write(buffer);

// 创建 HTTP 服务器
const httpServer = Http.createServer((req, res) => {
    switch (req.method) {
        case 'GET':
            // 处理 GET 请求
            console.log('Received GET request');
            res.writeHead(200, {'Content-Type': 'text/plain'});
            res.end('Received a GET request');
            break;

        case 'POST':
            // 处理 POST 请求
            console.log('Received POST request');
            let body = '';
            req.on('data', chunk => {
                body += chunk.toString(); // 转换 Buffer 到字符串
            });
            req.on('end', () => {
                try {
                    // 解析接收到的JSON数据
                    const jsonData = JSON.parse(body);

                    // 假设我们已经有了objId和httpMethod
                    const objId = 1; // 示例objId
                    const httpMethod = 1; // 假设1代表POST

                    // 使用convertJsonToCStruct函数转换JSON数据
                    const buffer = convertJsonToCStruct(jsonData, objId, httpMethod);

                    // 通过TCP客户端发送转换后的字节流到C语言服务器
                    g_tcpClient.write(buffer, () => {
                        res.writeHead(200, {'Content-Type': 'text/plain'});
                        res.end('Received a POST request with data and forwarded to C server');
                    });
                } catch (error) {
                    console.error('Error processing POST request:', error);
                    res.writeHead(500, {'Content-Type': 'text/plain'});
                    res.end('Error processing your request');
                }
            });
            break;

        case 'PUT':
            // 处理 PUT 请求
            console.log('Received PUT request');
            res.writeHead(200, {'Content-Type': 'text/plain'});
            res.end('Received a PUT request');
            break;

        default:
            // 处理其他类型的请求
            console.log(`Received ${req.method} request`);
            res.writeHead(405, {'Content-Type': 'text/plain'});
            res.end(`${req.method} method not allowed`);
    }
});

const httpPort = 8088;
httpServer.listen(httpPort, () => {
    console.log(`HTTP server listening on port ${httpPort}`);
});
