*** Settings ***
Library             Process
Library             String
Library             SerialLibrary
Library             CSVLibrary

Suite Teardown      Terminate All Processes    kill=True


*** Variables ***
${csv_file}     event_manager_benchmark_256kb.csv


*** Tasks ***
Clear Old CSV File
    Empty Csv File    ${csv_file}

 Event Manager Benchmark
    FOR    ${alloc}    IN    0    1
        FOR    ${consumers}    IN    1    2    4    8
            FOR    ${msg_size}    IN    1    2    4    8    16    32    64    128    256
                Benchmark Report For
                ...    message_size=${msg_size}
                ...    one_to=${consumers}
                ...    asynchronous=0
                ...    dynamic_alloc=${alloc}
            END
        END
    END

#Test 1
#    Benchmark Report For    message_size=256    one_to=1    asynchronous=1    dynamic_alloc=1


*** Keywords ***
Run Memory Report
    [Arguments]    ${type}
    ${result}    Run Process    west build -t ${type}_report    shell=True
    Should Be Equal As Integers    ${result.rc}    0
    ${mem}    Get Substring    ${result.stdout}    -20
    ${mem}    Strip String    ${mem}
    ${mem}    Convert To Integer    ${mem}
    RETURN    ${mem}

Measure Results
    ${total}    Set Variable    0
    Add Port    /dev/ttyACM0    timeout=600    baudrate=115200
    Set Encoding    ascii
    FOR    ${count}    IN RANGE    3
        ${result}    Run Process    west flash    shell=True
        Should Be Equal As Integers    ${result.rc}    0
        ${val}    Read Until    expected=@    encoding=ascii
        Log To Console    ${val}
        ${val}    Read Until    encoding=ascii
        Log To Console    ${val}
        ${val}    Strip String    ${val}
        ${val}    Convert To Integer    ${val}
        ${total}    Evaluate    ${total}+${val}
    END
    ${duration}    Evaluate    ${total}/3.0
    RETURN    ${duration}
    [Teardown]    Delete All Ports

Benchmark
    [Arguments]    ${message_size}=256    ${one_to}=1    ${asynchronous}=0    ${dynamic_alloc}=0
    ${result}    Run Process
    ...    make rebuild MSG_SIZE\=${message_size} ONE_TO\=${one_to} ASYNC\=${asynchronous} DALLOC\=${dynamic_alloc}
    ...    shell=True
    Should Be Equal As Integers    ${result.rc}    0
    ${duration}    Measure Results
    RETURN    ${duration}

Benchmark Report For
    [Arguments]    ${message_size}=256    ${one_to}=1    ${asynchronous}=0    ${dynamic_alloc}=0
    ${duration}    Benchmark
    ...    message_size=${message_size}
    ...    one_to=${one_to}
    ...    asynchronous=${asynchronous}
    ...    dynamic_alloc=${dynamic_alloc}
    ${ram_amount}    Run Memory Report    ram
    ${rom_amount}    Run Memory Report    rom
    IF    ${asynchronous}
        ${async_str}    Set Variable    ASYNC
    ELSE
        ${async_str}    Set Variable    SYNC
    END

    IF    ${dynamic_alloc}
        ${alloc_str}    Set Variable    DYN
    ELSE
        ${alloc_str}    Set Variable    STA
    END
    @{results}    Create List
    ...    ${alloc_str}
    ...    ${async_str}
    ...    ${one_to}
    ...    ${message_size}
    ...    ${duration}
    ...    ${ram_amount}
    ...    ${rom_amount}
    Log To Console    \n${results}
    Append To Csv File    ${csv_file}    ${results}
