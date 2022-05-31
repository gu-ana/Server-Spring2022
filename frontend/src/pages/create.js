import React, { useState, useEffect } from 'react';
import {
  Heading,
  Button,
  Input,
  RadioGroup,
  Radio,
  HStack,
  VStack,
  Image,
  FormControl,
  FormLabel,
  FormErrorMessage,
  FormHelperText,
  Alert,
  AlertIcon,
  AlertTitle,
  AlertDescription,
} from '@chakra-ui/react';
const CreateContainer = () => {
  // const [data, setData] = useState({});
  // const [error, setError] = useState("");
  // async function create_palette() {
  //   const res = await fetch(URL, {
  //     method: 'POST',
  //   })
  //     .then(response => {
  //       if (response.ok) {
  //         return response.json();
  //       }
  //     })
  //     .catch(error => {
  //       return error;
  //     });
  //   setData(res);
  // }
  return (
    <div className="create">
      <h1>Creation form goes here...</h1>
      {/* <Button onClick={() => { createMeme("hello") }}>Test Create</Button> */}
      <Button>Click me</Button>
      <p></p>
    </div>
  );
};
export default CreateContainer;