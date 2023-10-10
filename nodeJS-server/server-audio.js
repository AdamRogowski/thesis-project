const express = require("express");
const socket = require("socket.io");
const fs = require("fs");
const path = require("path");
const ffmpeg = require("fluent-ffmpeg");
const app = express();
var PORT = process.env.PORT || 3000;
const server = app.listen(PORT);
const storagePath =
  "C:\\Users\\user\\Desktop\\storage\\";

const SAMPLING_RATE_IN_HZ = "12000";
const AUDIO_FORMAT = "u8"; //AudioFormat.ENCODING_PCM_8BIT unsigned
//const AUDIO_FORMAT = "s16le "; //AudioFormat.ENCODING_PCM_16BIT little endian
const CHANNEL_CONFIG = 1; //AudioFormat.CHANNEL_IN_MONO

var testIterator = 0;

app.use(express.static("public"));
console.log("Server is running on port " + PORT);
const io = socket(server);

io.on("connection", (socket) => {
  console.log("New socket connection: " + socket.id);
  testIterator = 0;

  const date = new Date();
  const dateString = `${date.getFullYear()}-${
    date.getMonth() + 1
  }-${date.getDate()} ${date.getHours()}-${date.getMinutes()}-${date.getSeconds()}`;

  const audioFileDir = path.join(storagePath, dateString);
  if (!fs.existsSync(audioFileDir)) {
    fs.mkdirSync(audioFileDir, { recursive: true });
  }

  const audioFileStream = fs.createWriteStream(
    path.join(audioFileDir, "audio.raw")
  );

  socket.on("audioData", (data) => {
    console.log(testIterator, ". Received audioData of size:", data.length);
    audioFileStream.write(data);
    testIterator++;
  });

  socket.on("disconnect", () => {
    console.log("A client has disconnected");
    audioFileStream.end();

    // Check if audio.raw file is not empty before converting to mp3
    const audioFilePath = path.join(audioFileDir, "audio.raw");
    const audioFileSize = fs.statSync(audioFilePath).size;
    if (audioFileSize > 0) {
      // Convert the raw audio data to an mp3 file using ffmpeg
      ffmpeg()
        .input(path.join(audioFileDir, "audio.raw"))
        .inputFormat(AUDIO_FORMAT)
        .audioChannels(CHANNEL_CONFIG)
        .inputOptions("-ar", SAMPLING_RATE_IN_HZ)
        //.outputOptions("-c:a", "libopus", "-compression_level", "0")
        //.outputOptions("-c:a", "flac", "-compression_level", "8")
        .outputOptions("-c:a", "libmp3lame", "-q:a", "0")
        .on("end", () => {
          console.log("Finished converting audio to mp3");
        })
        .on("error", (err) => {
          console.error("Error converting audio to mp3:", err);
        })
        //.save(path.join(audioFileDir, "audio.ogg"));
        //.save(path.join(audioFileDir, "audio.flac"));
        .save(path.join(audioFileDir, "audio.mp3"));
    } else {
      console.log("No data sent by client");
    }
  });
});
