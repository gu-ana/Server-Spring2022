
import './App.css';
import React from 'react';

import { ChakraProvider , theme, 
  Tabs, TabList, TabPanels, Tab, TabPanel } from '@chakra-ui/react';

import CreateContainer from './pages/create';
function App() {
  return (
    <ChakraProvider theme={theme}>
      <Tabs>
        <TabList>
          <Tab> Generate a color palette </Tab>
          <Tab> Create a spotify playlist </Tab>
        </TabList>
        <TabPanels>
          <TabPanel> <CreateContainer /> </TabPanel>
          <TabPanel> Type a phrase/word to get started: </TabPanel>
        </TabPanels>
      </Tabs>
    </ChakraProvider>
  );
}


export default App;
