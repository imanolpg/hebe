#include "ast/ast.h"
#include <gtest/gtest.h>

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char* str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer);
extern int yyparse();

extern ASTNode* root;

TEST(Parsing, number_node_parsing) {
  std::string testCode = "1.0";

  // 1. Create a Flex buffer from the string
  YY_BUFFER_STATE buffer = yy_scan_string(testCode.c_str());

  // 2. Switch Flex to this buffer
  yy_switch_to_buffer(buffer);

  // 3. Run the parser
  int result = yyparse();

  EXPECT_EQ(result, 0);

  // 4. Clean up
  yy_delete_buffer(buffer);

  ProgramNode* program = static_cast<ProgramNode*>(root);

  // The program consists only of one item.
  EXPECT_EQ(program->getItems().size(), 1);
}