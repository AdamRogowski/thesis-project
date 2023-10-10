const express = require("express");
const WebSocket = require("ws");
const http = require("http");
const fs = require("fs");
const path = require("path");
const ffmpeg = require("fluent-ffmpeg");
const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });
const PORT = process.env.PORT || 2137;

const storagePath = "C:\\Users\\jaro\\Desktop\\webServer\\storage\\";

let frameIndex = 0;
let framesDir;

wss.on("connection", (ws) => {
  console.log("New WebSocket connection");

  const date = new Date();
  const dateString = `${date.getFullYear()}-${
    date.getMonth() + 1
  }-${date.getDate()} ${date.getHours()}-${date.getMinutes()}-${date.getSeconds()}`;

  framesDir = path.join(storagePath, dateString, "frames");
  if (!fs.existsSync(framesDir)) {
    fs.mkdirSync(framesDir, { recursive: true });
  }

  ws.on("message", (data) => {
    console.log(`Received video data of size: ${data.length}`);
    const filename = path.join(framesDir, `${frameIndex}.jpeg`);
    fs.writeFile(filename, data, (err) => {
      if (err) throw err;
      console.log(`Saved ${filename}`);
      frameIndex++;
    });
  });

  ws.on("close", () => {
    console.log("A client has disconnected");
    frameIndex = 0;

    // Convert the saved JPEG frames to an mp4 file using ffmpeg
    ffmpeg()
      .input(path.join(framesDir, "%d.jpeg"))
      .inputOptions("-framerate", "7")
      .outputOptions("-c:v", "libx264")
      .on("end", () => {
        console.log("Finished converting frames to mp4");
      })
      .on("error", (err) => {
        console.error("Error converting frames to mp4:", err);
      })
      .save(path.join(storagePath, dateString, "video.mp4"));
  });
});

server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
