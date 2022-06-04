
import './App.css';
import React from 'react';

import { ChakraProvider , theme, Heading, 
  Tabs, TabList, TabPanels, Tab, TabPanel } from '@chakra-ui/react';

import CreateContainer from './pages/create';
import ListContainer from './pages/list';
function App() {
  return (
    <ChakraProvider theme={theme}>
      <Heading> Team Swan Assignment 9 </Heading>
      <Tabs>
        <TabList>
          <Tab> Generate a color palette </Tab>
          <Tab> See past palettes </Tab>
        </TabList>
        <TabPanels>
          <TabPanel> <CreateContainer /> </TabPanel>
          <TabPanel> <ListContainer /> </TabPanel>
        </TabPanels>
      </Tabs>
    </ChakraProvider>
  );
}


export default App;
