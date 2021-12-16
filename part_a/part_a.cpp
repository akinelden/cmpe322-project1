// The project aims to communicate with a child process through pipes.
// stdin, stdout and stderr of the child process are binded with three pipes
// and parent process sends inputs and takes outputs through corresponding pipes.
//
// @author: Mehmet Akın Elden

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// In order to avoid mistakes, first define constants for pipe ends
#define READ_END 0
#define WRITE_END 1

// also define the buffer size to be used while reading from pipe
#define BUFFER_SIZE 1024

// actions taken by the child(blackbox) process: pipes are binded to stdin,
// stdout and stderr. Unused ends of the pipes are closed, and the blackbox program
// is loaded.
//  inputPipe: the pipe which will be binded to stdin of the process
//  outputPipe: the pipe which will be binded to stdout of the process
//  errorPipe: the pipe which will be binded to stderr of the process
//  blackbox: the path of the blackbox executable
int child_process(int inputPipe[2], int outputPipe[2], int errorPipe[2], string &blackbox)
{
    // bind read end of the inputPipe to stdin, exit if an error occurs
    if (dup2(inputPipe[READ_END], STDIN_FILENO) < 0)
        return -1;

    // bind write end of the outputPipe to stdout, exit if an error occurs
    if (dup2(outputPipe[WRITE_END], STDOUT_FILENO) < 0)
        return -1;

    // bind write end of the errorPipe to stderr, exit if an error occurs
    if (dup2(errorPipe[WRITE_END], STDERR_FILENO) < 0)
        return -1;

    // close the unused ends of pipes
    close(inputPipe[WRITE_END]);
    close(outputPipe[READ_END]);
    close(errorPipe[READ_END]);

    // binding is completed, load the blackbox and let it do the magic
    if (execl(blackbox.c_str(), blackbox.c_str(), NULL) < 0)
    {
        return -1;
    }

    return 0;
}

// actions taken by parent process: Unused ends of the pipes are closed,
// two input integers are taken from user, written to input pipe,
// and result is read from stdout if child process exits successfully,
// or result it read from stderr if child exits with error.
// then the result is written to output file with success or fail status.
//  inputPipe: the pipe which is binded to stdin of the child process
//  outputPipe: the pipe which is binded to stdout of the child process
//  errorPipe: the pipe which is binded to stderr of the child process
//  outputPath: the path of the output file in which the results of the blackbox process is written
int parent_process(int inputPipe[2], int outputPipe[2], int errorPipe[2], string &outputPath)
{
    // close the read end of input and write end of output/error pipes
    close(inputPipe[READ_END]);
    close(outputPipe[WRITE_END]);
    close(errorPipe[WRITE_END]);

    // read two integers from stdin convert them to single line of input
    int first, second;
    cin >> first;
    cin >> second;
    string input = to_string(first) + " " + to_string(second) + "\n";

    // try to write to inputPipe, if cannot write return -1
    if (write(inputPipe[WRITE_END], input.c_str(), input.size() + 1) <= 0)
        return -1;

    // open the output file
    fstream outputFile;
    outputFile.open(outputPath, ios::app);

    // wait for child process to finish its job and get the return status
    int status;
    wait(&status);

    // if the exit status of child is not 0, then an error occurred
    bool normalExit = false;
    if (WIFEXITED(status))
    {
        int exitStatus = WEXITSTATUS(status);
        normalExit = exitStatus == 0;
    }

    if (normalExit) // if child terminated normally, read from outputPipe (stdout)
    {
        char buff[BUFFER_SIZE] = {}; // the buffer to read from pipe
        // try to read the result from outputPipe
        if (read(outputPipe[READ_END], buff, sizeof(buff)) > 0)
        {
            // if successfully read, get the integer value from buff
            int result = stoi(string(buff));
            // first write SUCCESS and then result to outputFile
            outputFile << "SUCCESS:" << endl;
            outputFile << result << endl;
        }
        else // if couldn't read, it means something is wrong, return -1
            return -1;
    }
    else // if not a normal exit, then read from errorPipe (stderr)
    {
        // use a string and accumulate pipe data inside it
        string errorMsg = "";
        while (true)
        {
            // initialize a char array and try to read from errorPipe
            char msg[BUFFER_SIZE] = {};
            int nRead; // number of bytes read from pipe
            nRead = read(errorPipe[READ_END], msg, BUFFER_SIZE);
            if (nRead > 0) // if some content is read, add it to errorMsg
            {
                errorMsg += string(msg);
                if (nRead < BUFFER_SIZE) // it means all content is read from pipe
                    break;
            }
            else // otherwise complete content is read, break the loop
                break;
        }

        if (errorMsg.size() > 0) // if some message is read, write it to outputFile
        {
            outputFile << "FAIL:" << endl;
            outputFile << errorMsg;
            if (errorMsg.back() != '\n')
                outputFile << endl;
        }
        else // if no data is read, something is wrong return -1
            return -1;
    }

    // close the output file, pipes and return
    outputFile.close();
    close(inputPipe[WRITE_END]);
    close(outputPipe[READ_END]);
    close(errorPipe[READ_END]);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
	{
		printf("blackbox and outputfile arguments are required.");
		return -1;
	}
    // the path of the blackbox executable
    string blackbox = string(argv[1]);
    // the path of the output text
    string outputPath = string(argv[2]);

    // Three pipes are required:
    int inputPipe[2];  // For sending the input data to child process. WRITE: parent, READ: child
    int outputPipe[2]; // For receiving the output from child process. WRITE: child, READ: parent
    int errorPipe[2];  // For receiving error message from child process. WRITE: child, READ: parent

    // try to open the pipes, if an error occurs, exit
    if (pipe(inputPipe) < 0 || pipe(outputPipe) < 0 || pipe(errorPipe) < 0)
        return -1;

    int pid = fork();
    if (pid == 0) // child process actions
    {
        return child_process(inputPipe, outputPipe, errorPipe, blackbox);
    }
    else // parent process actions
    {
        return parent_process(inputPipe, outputPipe, errorPipe, outputPath);
    }
}