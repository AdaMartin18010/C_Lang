//
// Auto-generated from: 15_Memory_Safe_Languages\02_Migration_Strategies\01_Incremental_Migration.md
// Line: 568
// Language: zig
// Block ID: 5caaffed
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

    pub fn parse(data: &[u8]) -> Result<(Self, usize), ParseError> {
        // 查找请求行结束
        let line_end = data.windows(2)
            .position(|w| w == b"\r\n")
            .ok_or(ParseError::Incomplete)?;

        let line = &data[..line_end];
        let consumed = line_end + 2;

        // 安全地分割字段
        let parts: Vec<&[u8]> = line.split(|&b| b == b' ').collect();
        if parts.len() != 3 {
            return Err(ParseError::Incomplete);
        }

        // 验证并转换方法
        let method = str::from_utf8(parts[0])
            .map_err(|_| ParseError::InvalidMethod)?;
        if !is_valid_method(method) {
            return Err(ParseError::InvalidMethod);
        }

        // 验证路径
        let path = str::from_utf8(parts[1])
            .map_err(|_| ParseError::InvalidPath)?;
        if path.len() > 2048 {
            return Err(ParseError::InvalidPath);
        }

        // 验证版本
        let version = str::from_utf8(parts[2])
            .map_err(|_| ParseError::InvalidVersion)?;

        Ok((HttpRequest {
            method: method.to_string(),
            path: path.to_string(),
            version: version.to_string(),
        }, consumed))
    }
}

fn is_valid_method(method: &str) -> bool {
    matches!(method, "GET" | "POST" | "PUT" | "DELETE" |
                     "HEAD" | "OPTIONS" | "PATCH")
}

// C FFI绑定
use std::ffi::{c_char, c_int, c_void};
use std::ptr;

#[repr(C)]
pub struct CHttpRequest {
    method: *mut c_char,
    path: *mut c_char,
    version: *mut c_char,
}

/// # Safety
/// 调用者必须确保data指针有效且长度为data_len
#[no_mangle]
pub unsafe extern "C" fn http_request_parse(
    data: *const u8,
    data_len: usize,
    out_request: *mut *mut CHttpRequest,
) -> c_int {
    if data.is_null() || out_request.is_null() {
        return -1;
    }

    let slice = std::slice::from_raw_parts(data, data_len);

    match HttpRequest::parse(slice) {
        Ok((req, _)) => {
            let c_req = Box::new(CHttpRequest {
                method: CString::new(req.method).unwrap().into_raw(),
                path: CString::new(req.path).unwrap().into_raw(),
                version: CString::new(req.version).unwrap().into_raw(),
            });
            *out_request = Box::into_raw(c_req);
            0
        }
        Err(_) => -1,
    }
}

/// # Safety
/// 调用者必须确保req是有效的且仅释放一次
#[no_mangle]
pub unsafe extern "C" fn http_request_free(req: *mut CHttpRequest) {
    if !req.is_null() {
        let req = Box::from_raw(req);
        drop(CString::from_raw(req.method));
        drop(CString::from_raw(req.path));
        drop(CString::from_raw(req.version));
    }
}
