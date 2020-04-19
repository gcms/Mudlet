//
// Created by gustavo on 19/04/2020.
//

#ifndef MUDLET_TEST_TMXPSENDTAGHANDLERTEST_H
#define MUDLET_TEST_TMXPSENDTAGHANDLERTEST_H

#include <QTest>
#include <TMxpTagParser.h>
#include "TMxpStubClient.h"
#include "TMxpSendTagHandler.h"

class TMxpSendTagHandlerTest : public QObject {
Q_OBJECT

private:

private slots:

    void testStaticText()
    {
        // <SEND "tell Zugg " PROMPT>Zugg</SEND>
        TMxpStubContext ctx;
        TMxpStubClient stub;

        TMxpTagParser parser;
        MxpStartTag* startTag = parser.parseStartTag("<SEND \"tell Zugg \" PROMPT>");
        MxpEndTag* endTag = parser.parseEndTag("</SEND>");

        TMxpSendTagHandler sendTagHandler;
        TMxpTagHandler& tagHandler = sendTagHandler;
        tagHandler.handleTag(ctx, stub, startTag);
        tagHandler.handleContent("Zugg");
        tagHandler.handleTag(ctx, stub, endTag);

        QCOMPARE(stub.mHrefs.size(), 1);
        QCOMPARE(stub.mHrefs[0], "printCmdLine([[tell Zugg ]])");

        QCOMPARE(stub.mHints.size(), 1);
        QCOMPARE(stub.mHints[0], "tell Zugg ");
    }

    void testSimpleSend()
    {
        // <SEND>north</SEND>
        TMxpStubContext ctx;
        TMxpStubClient stub;

        MxpStartTag startTag("SEND");
        MxpEndTag endTag("SEND");

        TMxpSendTagHandler sendTagHandler;
        TMxpTagHandler& tagHandler = sendTagHandler;

        tagHandler.handleTag(ctx, stub, &startTag);
        tagHandler.handleContent("north");
        tagHandler.handleTag(ctx, stub, &endTag);

        QCOMPARE(stub.mHrefs.size(), 1);
        QCOMPARE(stub.mHrefs[0], "send([[north]])");

        QCOMPARE(stub.mHints.size(), 1);
        QCOMPARE(stub.mHints[0], "north");
    }

    void testSendPrompt()
    {
        // <SEND href="&text;" PROMPT>north</SEND>
        TMxpStubContext ctx;
        TMxpStubClient stub;

        TMxpTagParser parser;
        MxpStartTag* startTag = parser.parseStartTag("<SEND href=\"&text;\" PROMPT>");
        MxpEndTag* endTag = parser.parseEndTag("</SEND>");

        TMxpSendTagHandler sendTagHandler;
        TMxpTagHandler& tagHandler = sendTagHandler;
        tagHandler.handleTag(ctx, stub, startTag);
        tagHandler.handleContent("north");
        tagHandler.handleTag(ctx, stub, endTag);

        QCOMPARE(stub.mHrefs.size(), 1);
        QCOMPARE(stub.mHrefs[0], "printCmdLine([[north]])");

        QCOMPARE(stub.mHints.size(), 1);
        QCOMPARE(stub.mHints[0], "north");
    }

    void testResolvingEntity()
    {
        // <SEND href="&text;" PROMPT>north</SEND>
        TMxpStubContext ctx;
        TMxpStubClient stub;

        ctx.getEntityResolver().registerEntity("&charName;", "Gandalf");

        TMxpTagParser parser;
        MxpStartTag* startTag = parser.parseStartTag("<SEND href=\"say I am &charName;\">");
        MxpEndTag* endTag = parser.parseEndTag("</SEND>");

        TMxpSendTagHandler sendTagHandler;
        TMxpTagHandler& tagHandler = sendTagHandler;
        tagHandler.handleTag(ctx, stub, startTag);
        tagHandler.handleContent("TAG CONTENT");
        tagHandler.handleTag(ctx, stub, endTag);

        QCOMPARE(stub.mHrefs.size(), 1);
        QCOMPARE(stub.mHrefs[0], "send([[say I am Gandalf]])");

        QCOMPARE(stub.mHints.size(), 1);
        QCOMPARE(stub.mHints[0], "say I am Gandalf");
    }

};

#endif //MUDLET_TEST_TMXPSENDTAGHANDLERTEST_H

