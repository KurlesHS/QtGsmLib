#include "simpleatcommand.h"
#include "atchat.h"

SimpleAtCommand::SimpleAtCommand(const QString &atCommand,
                                 QObject *parent) :
    AtCommand(atCommand, parent)
{
}


bool SimpleAtCommand::processLine(QString line, AtChat * const chat)
{
    if (line == okResponce()) {
        setValid(true);
        return false;
    } else if (line == errorResponce()) {
        setValid(false);
        return false;
    }
    if (!line.trimmed().isEmpty()) {
        atResult().appendToContent(line);
    }
    afterSendCommand(chat);
    return true;
}

AtResult SimpleAtCommand::getCommandResult()
{

    AtResult res = atResult();
    res.setResultCode(isValid() ? AtResult::OK : AtResult::Error);
    return res;
}


void SimpleAtCommand::afterSendCommand(AtChat * const chat)
{
    chat->setWaitDataTimeout(1000);
}
