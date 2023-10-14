import express from "express";
import dotenv from "dotenv";
import cors from "cors";
import { Configuration, OpenAIApi } from "openai";

dotenv.config();

const configuration = new Configuration({
  apiKey: process.env.API_KEY,
});

const openai = new OpenAIApi(configuration);

const app = express();
app.use(cors());
app.use(express.json());

app.get("/", async (req, res) => {
  res.status(200).send({
    message: "Hello",
  });
});

let messages = [];

app.post("/", async (req, res) => {
  try {
    const prompt = req.body.prompt;

    const newMessage = {
      role: "user",
      content: prompt,
    };

    messages.push(newMessage);

    const completion = await openai.createChatCompletion({
      model: "gpt-3.5-turbo",
      messages: messages,
    });
    const botMessage = {
      role: "assistant",
      content: completion.data.choices[0].message.content,
    };
   messages.push(botMessage);

    const botResponse = botMessage.content; // Get the content of the bot response

    res.status(200).send({bot: botResponse});
  } catch (error) {
    console.error(error);
    res.status(500).send(error || "Something went wrong");
  }
});

app.listen(5000, () =>
  console.log("AI server started on http://localhost:5000")
);

