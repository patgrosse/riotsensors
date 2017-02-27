/*
 *  riotsensors - RIOT-OS module for sensor data transfers
 *
 *  Copyright (C) 2017 Patrick Grosse <patrick.grosse@uni-muenster.de>
 */

#include <rs_rest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

void print_result(rapidjson::Writer<rapidjson::StringBuffer> *writer, const rs_registered_lambda *lambda,
                  const generic_lambda_return *result) {
    switch (lambda->type) {
        case RS_LAMBDA_INT:
            writer->Int(result->ret_i);
            break;
        case RS_LAMBDA_DOUBLE:
            writer->Double(result->ret_d);
            break;
        case RS_LAMBDA_STRING:
            writer->String(result->ret_s);
            break;
        default:
            break;
    }
}

void print_lambda_properties(rapidjson::Writer<rapidjson::StringBuffer> *writer, const rs_registered_lambda *lambda) {
    writer->StartObject();
    writer->Key("id");
    writer->Uint(lambda->id);
    writer->Key("name");
    writer->String(lambda->name);
    writer->Key("type");
    {
        writer->StartObject();
        writer->Key("code");
        writer->Uint(lambda->type);
        writer->Key("string");
        writer->String(stringify_rs_lambda_type_t(lambda->type));
        writer->EndObject();
    }
    writer->Key("cache");
    {
        writer->StartObject();
        writer->Key("policy");
        writer->Uint(lambda->cache);
        writer->Key("string");
        writer->String(stringify_rs_cache_type_t(lambda->cache));
        writer->EndObject();
    }
    writer->EndObject();
}

void print_lambda_type_n_cache_unknown(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
    writer->Key("type");
    {
        writer->StartObject();
        writer->Key("code");
        writer->Int(-1);
        writer->Key("string");
        writer->String("unknown");
        writer->EndObject();
    }
    writer->Key("cache");
    {
        writer->StartObject();
        writer->Key("policy");
        writer->Int(-1);
        writer->Key("string");
        writer->String("unknown");
        writer->EndObject();
    }
}

void print_cache_content(rapidjson::Writer<rapidjson::StringBuffer> *writer, const rs_registered_lambda *lambda) {
    rs_linux_registered_lambda *arg = (rs_linux_registered_lambda *) lambda->arg;
    writer->StartObject();
    writer->Key("cache_available");
    writer->Bool(arg->data_cached);
    if (arg->data_cached) {
        writer->Key("cached_result");
        print_result(writer, lambda, &arg->ret);
    }
    writer->EndObject();
}

std::string
assemble_call_success_rest(rs_registered_lambda *lambda, bool cache_retrieved, bool timeout,
                           generic_lambda_return *result) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(true);
    writer.Key("lambda");
    {
        print_lambda_properties(&writer, lambda);
    }
    writer.Key("cache");
    {
        writer.StartObject();
        writer.Key("retrieved");
        writer.Bool(cache_retrieved);
        writer.Key("timeout");
        writer.Bool(timeout);
        writer.EndObject();
    }
    writer.Key("result");
    print_result(&writer, lambda, result);
    writer.EndObject();
    return s.GetString();
}

std::string assemble_call_error_rest_id(lambda_id_t id, const rs_registered_lambda *lambda, int8_t error) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(false);
    writer.Key("lambda");
    {
        if (lambda != NULL) {
            print_lambda_properties(&writer, lambda);
        } else {
            writer.StartObject();
            writer.Key("id");
            writer.Uint(id);
            writer.Key("name");
            writer.String("unknown");
            print_lambda_type_n_cache_unknown(&writer);
            writer.EndObject();
        }
    }
    writer.Key("error");
    {
        writer.StartObject();
        writer.Key("code");
        writer.Int(error);
        writer.Key("string");
        writer.String(stringify_rs_call_result(error));
        writer.EndObject();
    }
    writer.EndObject();
    return s.GetString();
}

std::string assemble_call_error_rest_name(std::string name, const rs_registered_lambda *lambda, int8_t error) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writer.StartObject();
    writer.Key("success");
    writer.Bool(false);
    writer.Key("lambda");
    {
        if (lambda != NULL) {
            print_lambda_properties(&writer, lambda);
        } else {
            writer.StartObject();
            writer.Key("id");
            writer.Int(-1);
            writer.Key("name");
            writer.String(name.c_str());
            print_lambda_type_n_cache_unknown(&writer);
            writer.EndObject();
        }
    }
    writer.Key("error");
    {
        writer.StartObject();
        writer.Key("code");
        writer.Int(error);
        writer.Key("string");
        writer.String(stringify_rs_call_result(error));
        writer.EndObject();
    }
    writer.EndObject();
    return s.GetString();
}

std::string assemble_list_rest() {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    lambda_id_t count = 0;
    writer.StartObject();
    writer.Key("lambdas");
    {
        writer.StartObject();
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                count++;
                char idstr[10];
                sprintf(idstr, "%d", lambda->id);
                writer.Key(idstr);
                {
                    print_lambda_properties(&writer, lambda);
                }
            }
        }
        writer.EndObject();
    }
    writer.Key("count");
    writer.Int(count);
    writer.EndObject();
    return s.GetString();
}

std::string assemble_list_rest_for_type(const rs_lambda_type_t type) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    lambda_id_t count = 0;
    writer.StartObject();
    writer.Key("lambdas");
    {
        writer.StartObject();
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                if (lambda->type != type) {
                    continue;
                }
                count++;
                char idstr[10];
                sprintf(idstr, "%d", lambda->id);
                writer.Key(idstr);
                {
                    print_lambda_properties(&writer, lambda);
                }
            }
        }
        writer.EndObject();
    }
    writer.Key("count");
    writer.Int(count);
    writer.EndObject();
    return s.GetString();
}

std::string assemble_cache_rest() {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    lambda_id_t count = 0;
    writer.StartObject();
    writer.Key("lambdas");
    {
        writer.StartObject();
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                count++;
                char idstr[10];
                sprintf(idstr, "%d", lambda->id);
                writer.Key(idstr);
                {
                    writer.StartObject();
                    writer.Key("lambda");
                    {
                        print_lambda_properties(&writer, lambda);
                    }
                    writer.Key("cache-content");
                    {
                        print_cache_content(&writer, lambda);
                    }
                    writer.EndObject();
                }
            }
        }
        writer.EndObject();
    }
    writer.Key("count");
    writer.Int(count);
    writer.EndObject();
    return s.GetString();
}

std::string assemble_cache_rest_for_type(const rs_lambda_type_t type) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    lambda_id_t count = 0;
    writer.StartObject();
    writer.Key("lambdas");
    {
        writer.StartObject();
        for (lambda_id_t i = 0; i < get_number_of_registered_lambdas(); i++) {
            rs_registered_lambda *lambda = get_registered_lambda_by_id(i);
            if (lambda != NULL) {
                if (lambda->type != type) {
                    continue;
                }
                count++;
                char idstr[10];
                sprintf(idstr, "%d", lambda->id);
                writer.Key(idstr);
                {
                    writer.StartObject();
                    writer.Key("lambda");
                    {
                        print_lambda_properties(&writer, lambda);
                    }
                    writer.Key("cache-content");
                    {
                        print_cache_content(&writer, lambda);
                    }
                    writer.EndObject();
                }
            }
        }
        writer.EndObject();
    }
    writer.Key("count");
    writer.Int(count);
    writer.EndObject();
    return s.GetString();
}