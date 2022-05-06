// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <boost/filesystem.hpp>

#include "config_parser.h"
#include "logger.h"

// helper function to check if input string are valid integer
bool is_digits(const std::string &str)
{
    if(str.size() == 0) {
        return false;
    }
    for(int i = 0; i < str.size(); i++) {
      if (str[i] < '0' || str[i] > '9') {
        return false;
      }
    }
    return true;
}


std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

int NginxConfig::getPort() {
  int port = -1;
  for (auto const& statement : statements_) {
    if (statement->child_block_.get() == nullptr) {
      if (statement->tokens_.size() == 2 && statement->tokens_[0] == "listen") {
	if (!is_digits(statement->tokens_[1].c_str())) {
	  LOG(error) << "  " << "Specified port number contained non-numeric values.\n";
	  return -1;
	}
        port = atoi(statement->tokens_[1].c_str());
        if (port >= 0 && port <= 65535) return port; //valid port num
	else {
	  LOG(error) << "  " << "Specified port number not in valid range [0-65535].\n";
	  return -1;
	}	
      }
    }
  }
  // statements with child blocks
  for (auto const& statement : statements_) {
    if (statement->child_block_.get() != nullptr) {
      if ((port = statement->child_block_->getPort()) != -1)
        return port;
    }
  }
  return -1;  
}

std::map<std::string, std::string> NginxConfig::get_filesystem_map() {
  return filesystem_map_;
}

bool NginxConfig::validate_relative_paths()
{
  bool all_relative_paths = true;
  
  for (const auto& statement : statements_) 
  {
    {
      if(statement->tokens_[0] == "server") 
      {
        if(statement->child_block_.get() != nullptr) 
        {
          for(const auto& block_line : statement->child_block_->statements_) 
          {
            if(block_line->tokens_[0] != "location") 
            {
              continue;
            }
            if(block_line->child_block_.get() != nullptr &&
               block_line->child_block_->statements_[0]->tokens_[0] == "root")
              {
                //check to see that the second token is a relative path
                //if not, we return and not all paths are relative
                boost::filesystem::path current_path(block_line->child_block_->statements_[0]->tokens_[1]);
                if(!current_path.is_relative()) 
                {
                  all_relative_paths = false;
                  return all_relative_paths;
                }
              }
          }
        }
      }
    }
    
  }
  return all_relative_paths; 
}

void NginxConfig::extract_filesystem_map() 
{
  for (const auto& statement : statements_) 
  {
    {
      if(statement->tokens_[0] == "server") 
      {
        if(statement->child_block_.get() != nullptr) 
        {
          for(const auto& block_line : statement->child_block_->statements_) 
          {
            if(block_line->tokens_[0] != "location") 
            {
              continue;
            }
            for(const auto& location_line : block_line->child_block_->statements_)
            {
              if(location_line->tokens_[0] == "root")
              {
                // location /static/ {
                //  root ./files;
                // }
                // maps location such as "/static/" to path specified by root "./files"
                filesystem_map_.insert({block_line->tokens_[1], location_line->tokens_[1]});
              }
            }
          }
        }
      }
    }
    
  }
  return; 
}

std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        *value += c;
        if (c == '\'') {
	  const char next_c = input->get();
	  // end of quoted token, followed by whitespace
          if (next_c == ' ' || next_c == '\t' || next_c == '\n' || next_c == '\t' ||
              next_c == ';' || next_c == '{' || next_c == '}') {
            input->unget();
          }
	  else {
	    return TOKEN_TYPE_ERROR;
	  }
          return TOKEN_TYPE_NORMAL;
        }
	// allow for backslash escape within single quote string
	else if (c == '\\') {
	  value -> pop_back();
	  state = TOKEN_STATE_SINGLE_QUOTE_ESCAPE;
	}
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
	  // same deal, but now whitespace with double quotes
	  const char next_c = input->get();
          if (next_c == ' ' || next_c == '\t' || next_c == '\n' || next_c == '\t' ||
              next_c == ';' || next_c == '{' || next_c == '}') {
            input->unget();
	  }
	  else { return TOKEN_TYPE_ERROR; }
          return TOKEN_TYPE_NORMAL;
        }
	// allow backslash escape again, but now for double quotes
	// mark state appropriately
	else if (c == '\\') {
	  value -> pop_back();
	  state = TOKEN_STATE_DOUBLE_QUOTE_ESCAPE;
	}
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
      case TOKEN_STATE_SINGLE_QUOTE_ESCAPE: // state for escape within single quote string
        *value += c;
	state = TOKEN_STATE_SINGLE_QUOTE;
	continue;
      case TOKEN_STATE_DOUBLE_QUOTE_ESCAPE: // same but for double quote
        *value += c;
	state = TOKEN_STATE_DOUBLE_QUOTE;
	continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_START_BLOCK &&
	  last_token_type != TOKEN_TYPE_END_BLOCK &&
	  last_token_type != TOKEN_TYPE_STATEMENT_END) {
        // Error. Unmatched Brace in Block.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_START &&  
	  last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.  Empty Config FIle
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  LOG(error) << "  " << "Bad transition from " << TokenTypeAsString(last_token_type) << "to" << TokenTypeAsString(token_type);
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    LOG(error) << "Failed to open config file: " << file_name;
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();

  if(!config->validate_relative_paths())
  {
    //config contains at least one absolute path
    LOG(error) << "Config file contains at least one absolute path. All paths must be relative. \n";
    return false;
  }
  return return_value;
}

